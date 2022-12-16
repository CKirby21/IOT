/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI Corporation
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example transmits data on hardcoded channel and receives data
 * when not transmitting. Running this sketch on two nodes should allow
 * them to communicate.
 *******************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "wiring_private.h" // pinPeripheral() function
#include <FlashStorage.h>

// we formerly would check this configuration; but now there is a flag,
// in the LMIC, LMIC.noRXIQinversion;
// if we set that during init, we get the same effect. If
// DISABLE_INVERT_IQ_ON_RX is defined, it means that LMIC.noRXIQinversion is
// treated as always set.
//
// #if !defined(DISABLE_INVERT_IQ_ON_RX)
// #error This example requires DISABLE_INVERT_IQ_ON_RX to be set. Update \
// lmic_project_config.h in arduino-lmic/project_config to set it.
// #endif
// How often to send a packet. Note that this sketch bypasses the normal
// LMIC duty cycle limiting, so when you change anything in this sketch
// (payload length, frequency, spreading factor), be sure to check if
// this interval should not also be increased.
// See this spreadsheet for an easy airtime and duty cycle calculator:
// https://docs.google.com/spreadsheets/d/1voGAtQAjC1qBmaVuP1ApNKs1ekgUjavHuVQIXyYSvNc
#define TX_INTERVAL 20000 //Delay between each message in millidecond.

FlashStorage(my_flash_store, int);

 // Pin mapping for SAMD21
 const lmic_pinmap lmic_pins = {
 .nss = 12,//RFM Chip Select
 .rxtx = LMIC_UNUSED_PIN,
 .rst = 7,//RFM Reset
 .dio = {6, 10, 11}, //RFM Interrupt, RFM LoRa pin, RFM LoRa pin
 };
// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmoc/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }

void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }
void onEvent (ev_t ev) {
}
osjob_t txjob;
osjob_t timeoutjob;
static void tx_func (osjob_t* job);

void setupWatchdog() {
  // Generic clock generator 2, divisor = 32 (2^(DIV+1))
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(6); // 3 gives a divisor of 16
  // Enable clock generator 2 using low-power 32KHz oscillator.
  // With /16 divisor above, this yields 2048Hz(ish) clock.
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(1) |
  GCLK_GENCTRL_GENEN |
  GCLK_GENCTRL_SRC_OSCULP32K |
  GCLK_GENCTRL_DIVSEL;
  while(GCLK->STATUS.bit.SYNCBUSY); // Think about why this is used
  // WDT clock = clock gen 2
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
  GCLK_CLKCTRL_CLKEN |
  GCLK_CLKCTRL_GEN_GCLK1;
  
  WDT->CTRL.reg = 0; // Disable watchdog for config
  
  WDT->CONFIG.bit.PER = 0xB; // Set period for chip reset from the datasheet, 0xA with 2048Hz clock gets 4s timer
  WDT->INTENCLR.bit.EW = 1; // Disable early warning interrupt
  WDT->CTRL.bit.WEN = 0; // Disable window mode
  
  WDT->CTRL.bit.ENABLE = 1;
}

// Transmit the given string and call the given function afterwards
void tx(const char *str, osjobcb_t func) {
 os_radio(RADIO_RST); // Stop RX first
 delay(1); // Wait a bit, without this os_radio below asserts, apparently because the state hasn't changed yet
 LMIC.dataLen = 0;
 while (*str)
 LMIC.frame[LMIC.dataLen++] = *str++;
 LMIC.osjob.func = func;
 os_radio(RADIO_TX);
 SerialUSB.println("TX");
}

// Enable rx mode and call func when a packet is received
void rx(osjobcb_t func) {
 LMIC.osjob.func = func;
 LMIC.rxtime = os_getTime(); // RX _now_
 // Enable "continuous" RX (e.g. without a timeout, still stops after
 // receiving a packet)
 os_radio(RADIO_RXON);
 SerialUSB.println("RX");
}

static void rxtimeout_func(osjob_t *job) {
 digitalWrite(LED_BUILTIN, LOW); // off

}
 
static void rx_func (osjob_t* job) {
 // Blink once to confirm reception and then keep the led on
 digitalWrite(LED_BUILTIN, LOW); // off
 delay(10);
 digitalWrite(LED_BUILTIN, HIGH); // on
 // Timeout RX (i.e. update led status) after 3 periods without RX os_setTimedCallback(&timeoutjob, os_getTime() + ms2osticks(3*TX_INTERVAL), rxtimeout_func);
 // Reschedule TX so that it should not collide with the other side's
 // next TX
 os_setTimedCallback(&txjob, os_getTime() + ms2osticks(TX_INTERVAL/2), tx_func);
 SerialUSB.print("Got ");
 SerialUSB.print(LMIC.dataLen);
 SerialUSB.println(" bytes");
 SerialUSB.write(LMIC.frame, LMIC.dataLen);
 SerialUSB.println();
 // Restart RX
 rx(rx_func);
}

static void txdone_func (osjob_t* job) {
 //rx(rx_func);
}

Uart mySerial (&sercom2, 3, 2, SERCOM_RX_PAD_1, UART_TX_PAD_2);
void SERCOM2_Handler()
{
  mySerial.IrqHandler();
}

char latString[9];
char lonString[11];
int errorCode = 0;
          
void getGPSLocation() {
  char c;
  char message[100];
  int iter = 0;

  char gpsType[5];
  char timestamp[12];
  char latitude[11];
  char vert;
  char longitude[12];
  char horz;
  char fixQual;
  char numSat[2];

  char y = 'n';
  while (y != '$') {
    y = mySerial.read();
  }
  while(1) {
    c = mySerial.read();
    
    if ((c < 128) && (c > 32)) {
      message[iter] = c;
      //SerialUSB.print(c);
      
      if (c == 36) {     
        int i = 0;
  
        //for (i = 0; i < iter; i++) {
        //  SerialUSB.print(message[i]);
        //}
        //SerialUSB.print("\n");
        int j = 0;   
        int notComma = 1;  
  
        while (notComma) {
          if (message[j] == 44) {
            gpsType[i] = '\0';
            notComma = 0;
          } else if (message[j] == 0) { // Sometimes 0 index was NULL character
            i = -1;
          } else {
            gpsType[i] = message[j];
          }
          i++;
          j++;
        }
        
        if (!strcmp(gpsType, "GPGGA")) {
          // Timestamp
          i = 0;
          notComma = 1;
          while(notComma) {
            if (message[j] == 44) {
              notComma = 0;
            }
            j++;
          }
  
          // latitude
          i = 0;
          notComma = 1;
          while(notComma) {
            if (message[j] == 44) {
              latitude[i] = '\0';
              notComma = 0;
            }
            latitude[i] = message[j];
            i++;
            j++;
          }
  
          // North or South
          if (message[j] != 44) {
            vert = message[j];
            j+=2;
          } else {
            vert = '\0';
            j++;
          }
  
          // Longitude
          i = 0;
          notComma = 1;
          while(notComma) {
            if (message[j] == 44) {
              longitude[i] = '\0';
              notComma = 0;
            }
            longitude[i] = message[j];
            i++;
            j++;
          }
  
          // West or East
          if (message[j] != 44) {
            horz = message[j];
            j+=2;
          } else {
            horz = '\0';
            j++;
          }
  
          // Sat fix quality
          fixQual = message[j];
          j+=2;
  
          // Number of connected satellites
          numSat[0] = message[j];
          numSat[1] = message[j+1];

          if (atoi(numSat) == 0) {
            errorCode = 1;
            SerialUSB.print("ahhhaahha no sat\n");
          }
  
          char latTemp[3];
          latTemp[0] = latitude[0];
          latTemp[1] = latitude[1];
          latTemp[2] = '\0';
          latitude[0] = '0';
          latitude[1] = '0';
          float latNum = atof(latTemp) + (atof(latitude))/60;
          if (vert == 'S') {
            latNum *= -1;
          }
  
          char lonTemp[4];
          lonTemp[0] = longitude[0];
          lonTemp[1] = longitude[1];
          lonTemp[2] = longitude[2];
          lonTemp[3] = '\0';
          longitude[0] = '0';
          longitude[1] = '0';
          longitude[2] = '0';
          float lonNum = atof(lonTemp) + (atof(longitude))/60;
          if (horz == 'W') {
            lonNum *= -1;
          }
  
          gcvt(latNum, 7, latString);
          gcvt(lonNum, 8, lonString);

          latString[9] = '\0';
          lonString[10] = '\0';
          
          //sprintf(locPacket, "%s,%s", latString, lonString);
          //SerialUSB.println(locPacket);
          return;
        }
        
        iter = 0;
        message[0] = '\0';
      }
      iter++;
    }
  }
}


// log text to USART and toggle LED
//float readTemperatureCount = 0;
//unsigned long timestamp = 0;
const int nodeID = 0;
int packetID = 0;
static void tx_func (osjob_t* job) {

  getGPSLocation();
 
  //timestamp = millis();
  char packet[100]; 
  sprintf(packet, "%d,%d,%s,%s", packetID, nodeID, latString, lonString);
  SerialUSB.println(packet);
  tx(packet, txdone_func);
  my_flash_store.write(errorCode);
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
  packetID++;
 
 // reschedule job every TX_INTERVAL (plus a bit of random to prevent
 // systematic collisions), unless packets are received, then rx_func
 // will reschedule at half this time.
 os_setTimedCallback(job, os_getTime() + ms2osticks(TX_INTERVAL + random(500)),
tx_func);
}

// application entry point
//TemperatureZero TempZero = TemperatureZero();
void setup() {
  mySerial.begin(9600);
  SerialUSB.begin(115200);
  while(!SerialUSB);
  pinPeripheral(2, PIO_SERCOM);
  pinPeripheral(3, PIO_SERCOM_ALT);

  pinMode(PIN_LED_13, OUTPUT);
  digitalWrite(PIN_LED_13, LOW);

  setupWatchdog();
  
  SerialUSB.println("Starting");
  //TempZero.init();
  // #ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  // pinMode(VCC_ENABLE, OUTPUT);
  // digitalWrite(VCC_ENABLE, HIGH);
  // delay(1000);
  // #endif
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize runtime env
  os_init();
  // this is automatically set to the proper bandwidth in kHz,
  // based on the selected channel.
  uint32_t uBandwidth;
  LMIC.freq = 903900000;
  uBandwidth = 125;
  LMIC.datarate = US915_DR_SF7; // DR4
  LMIC.txpow = 21;

  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;
  // This sets CR 4/5, BW125 (except for EU/AS923 DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);
  SerialUSB.print("Frequency: "); SerialUSB.print(LMIC.freq / 1000000);
  SerialUSB.print("."); SerialUSB.print((LMIC.freq / 100000) % 10);
  SerialUSB.print("MHz");
  SerialUSB.print(" LMIC.datarate: "); SerialUSB.print(LMIC.datarate);
  SerialUSB.print(" LMIC.txpow: "); SerialUSB.println(LMIC.txpow);
  // This sets CR 4/5, BW125 (except for DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);
  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;
  SerialUSB.println("Started");
  SerialUSB.flush();
  // setup initial job
  os_setCallback(&txjob, tx_func);
}

int counter = 0;
void loop() {
  /*
  static unsigned int counter = 0;
  if (counter % 10000 == 0) {
    averageTemperature += TempZero.readInternalTemperature();
    readTemperatureCount++;
  }
  counter++;
  */
  if (counter >= 4) {
    digitalWrite(PIN_LED_13, HIGH);
    delay(750);
    digitalWrite(PIN_LED_13, LOW);
    counter = 0;
  } else {
    delay(1000);
    counter++;
  }

 
  
  // execute scheduled jobs and events
  os_runloop_once();
}
