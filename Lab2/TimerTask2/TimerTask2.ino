// LED definitions in the datasheet
// D13 (PIN_LED_13): Blue
// TX (PIN_LED_TXL): Green
// RX (PIN_LED_RXL): Yellow
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024
void startBlueTimer(int frequencyHz);
void startYellowTimer(int frequencyHz);
void setTimerFrequency(TcCount16* TC, int frequencyHz);
void setTimerRegisters(TcCount16* TC, int frequencyHz);
void TC3_Handler();
void TC4_Handler();
bool isBlueOn = false;
bool isYellowOn = false;

void setup() {
 SerialUSB.begin(9600);
 // while(!SerialUSB);
 pinMode(PIN_LED_13, OUTPUT);
 pinMode(PIN_LED_RXL, OUTPUT);
 digitalWrite(PIN_LED_RXL, HIGH);
 startTimerBlue(2);
 startTimerYellow(1);
}
void loop() {}

void setTimerFrequency(TcCount16* TC, int frequencyHz) {
  int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  SerialUSB.print("TC Count register: ");
  SerialUSB.println(TC->COUNT.reg);
  SerialUSB.print("TC CC register: ");
  SerialUSB.println(TC->CC[0].reg);
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void startTimerBlue(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
  TcCount16* TC = (TcCount16*) TC3;

  setTimerRegisters(TC, frequencyHz);
  setTimerFrequency(TC, frequencyHz);
  
  // Enable the compare interrupt
  TC->INTENSET.reg = 0; // This clears the entire register?
  TC->INTENSET.bit.MC0 = 1; // This enables the MCO?
  NVIC_EnableIRQ(TC3_IRQn);
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

void startTimerYellow(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC4_TC5) ; // MINE
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
  TcCount16* TC = (TcCount16*) TC4;
  
  setTimerRegisters(TC, frequencyHz);
  setTimerFrequency(TC, frequencyHz);
  
  // Enable the compare interrupt
  TC->INTENSET.reg = 0; // This clears the entire register?
  TC->INTENSET.bit.MC0 = 1; // This enables the MCO?  
  NVIC_EnableIRQ(TC4_IRQn);
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

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

// TODO Need to use the overflow interrupt instread of the match interrupt
void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    // Write callback here!!!
    digitalWrite(PIN_LED_13, isBlueOn);
    isBlueOn = !isBlueOn;
  }
}
void TC4_Handler() {
  TcCount16* TC = (TcCount16*) TC4;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    // Write callback here!!!
    digitalWrite(PIN_LED_RXL, isYellowOn);
    isYellowOn = !isYellowOn;
  }
}
