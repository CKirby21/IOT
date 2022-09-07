// LED definitions in the datasheet
// D13 (PIN_LED_13): Blue
// TX (PIN_LED_TXL): Green
// RX (PIN_LED_RXL): Yellow
#define CPU_HZ 48000000
// TODO Change this
#define TIMER_PRESCALER_DIV 1024
void startTimer(int frequencyHz);
void setTimerFrequency(TcCount8* TC, int frequencyHz);
void TC3_Handler();
bool isBlueOn = false;
//bool isYellowOn = false;
int interruptCount = 0;

void setup() {
 SerialUSB.begin(9600);
 // while(!SerialUSB);
 pinMode(PIN_LED_13, OUTPUT);
// pinMode(PIN_LED_RXL, OUTPUT);
// digitalWrite(PIN_LED_RXL, HIGH);
 startTimer(1);
}
void loop() {}

void setTimerFrequency(TcCount8* TC, int frequencyHz) {
  
  // TODO Write to 8-bit counter's Period register (PER) to control the top value
  int periodValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;

  // TODO Something like this?
  TC->PER.reg = periodValue; 
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.

  // TODO Not sure I need this
  // TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  SerialUSB.print("TC Count register: ");
  SerialUSB.println(TC->COUNT.reg);
  SerialUSB.print("TC Period register: ");
  SerialUSB.println(TC->PER.reg);
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
  TcCount8* TC = (TcCount8*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE; //Disable timer
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
  // Use the 8-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT8;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // TODO Use Normal Mode (NFRQ)
  // When MFRQ is used, the value in CC0 will be used as the top value and WO[0] will toggle on every overflow/underflow.
  // When NFRQ is used, the waveform output (WO[x]) toggles every time CCx and the counter are equal, and the interrupt flag corresponding to that channel will be set.
  
  // TODO Change this
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  setTimerFrequency(TC, frequencyHz);
  
  // Enable the compare interrupt
  TC->INTENSET.reg = 0; // This clears the entire register?
  TC->INTENSET.bit.OVF = 1; 
  NVIC_EnableIRQ(TC3_IRQn);
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

// TODO Need to use the overflow interrupt instread of the match interrupt
void TC3_Handler() {
  TcCount8* TC = (TcCount8*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.OVF == 1) {
    TC->INTFLAG.bit.OVF = 1;
    // Write callback here!!!
    digitalWrite(PIN_LED_13, isBlueOn);
    isBlueOn = !isBlueOn;
  }
}
