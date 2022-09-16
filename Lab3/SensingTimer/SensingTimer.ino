// LED definitions in the datasheet
// D13 (PIN_LED_13): Blue
// TX (PIN_LED_TXL): Green
// RX (PIN_LED_RXL): Yellow

#include <SPI.h>
#include <RH_RF95.h>
#include <TemperatureZero.h>

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024
void startTimer(int frequencyHz);
void setWatchdog();
void setTimerFrequency(TcCount16* TC, int frequencyHz);
void setTimerRegisters(TcCount16* TC, int frequencyHz);
void TC4_Handler();

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

TemperatureZero TempZero = TemperatureZero();

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
float frequency = 906; //Broadcast frequency

int LED = 13; //Status LED is on pin 13
int secondsElapsed = 0;
// Node ID is specific to each sensor device
const int nodeID = 1;
int packetID = 1;
int timestamp;
float averageTemperature = 0;
int exitCode = 0;


void setup() {
  SerialUSB.begin(9600);
  while(!SerialUSB);
  
  TempZero.init();

  int delayTime = nodeID * 1000;
  
  SerialUSB.println("RFM Client!");

  //Initialize the Radio.
  if (rf95.init() == false) {
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  } else {
    //An LED inidicator to let us know radio initialization has completed.
    SerialUSB.println("Transmitter up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  rf95.setFrequency(frequency);

  // Transmitter power can range from 14-20dbm.
  rf95.setTxPower(20, false);

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  
  // Wait until Leader transmits "Start"
  while(true) {
    if(rf95.available()) {
      if (rf95.recv(buf, &len)) {
        if (!strcmp((char *)buf, "Start")) {
          break;
        }
      }
    }
  }
  
  delay(delayTime);
  startTimer(1);
  setWatchdog();
}
void loop() {}

void setWatchdog() {
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4); 
  // Enable clock generator 2 using low-power KHz oscillator. 
  // With /64 divisor above, this yields Hz(ish) clock. 
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | 
                      GCLK_GENCTRL_GENEN | 
                      GCLK_GENCTRL_SRC_OSCULP32K | 
                      GCLK_GENCTRL_DIVSEL; 

  while(GCLK->STATUS.bit.SYNCBUSY);
  
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_TC6_TC7);

  WDT->CTRL.reg = 0;

  // WDT counts down from 4096
  WDT->CONFIG.bit.PER    = 0x9;   // Set period for chip reset from the datasheet
  // Early warning counts down from 2048
  WDT->EWCTRL.reg        = 0x08;
  WDT->INTENSET.bit.EW   = 1;      // Enable early warning interrupt
  WDT->CTRL.bit.WEN      = 0;      // Disable window mode

  WDT->CTRL.bit.ENABLE = 1;
}

// Sets ClkGen0 to TC4
void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC4_TC5) ; // MINE
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
  TcCount16* TC = (TcCount16*) TC4;
  
  setTimerRegisters(TC, frequencyHz);
  setTimerFrequency(TC, frequencyHz);
  
  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.OVF = 1;  
  NVIC_EnableIRQ(TC4_IRQn);
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

// Sets TC4 to 16 bit match mode
void setTimerRegisters(TcCount16* TC, int frequencyHz) {
  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE; //Disable timer
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
  
  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
  // Set prescaler to 1024
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

// Sets TC4 to 1 sec interval
void setTimerFrequency(TcCount16* TC, int frequencyHz) {
  int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

// Averages temperature ands transmits every 5 sec
void TC4_Handler() {
  TcCount16* TC = (TcCount16*) TC4;
  if (TC->INTFLAG.bit.OVF == 1) {
    TC->INTFLAG.bit.OVF = 1;
    secondsElapsed++;
    averageTemperature += TempZero.readInternalTemperature();
    if (secondsElapsed % 5 == 0) {
      timestamp = millis();
      averageTemperature /= 5;
      
      // Convert to String because sprintf doesnt support floats
      char payload[10];
      gcvt(averageTemperature, 6, payload);

      // Read Early Warning interrupt bit
      if (WDT->INTFLAG.bit.EW) {
         exitCode = 1;
      }

      // Create packet string
      char packet[100]; 
      sprintf(packet, "%d,%d,%d,%s,%d", nodeID, packetID, timestamp, payload, exitCode);
      SerialUSB.println(packet);
      
      // Send the packet to the leader
      rf95.send((uint8_t *)packet, sizeof(packet));
      
      // Wait for response
//      rf95.waitPacketSent();

      averageTemperature = 0;
      packetID++;
    }
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
  }
}
