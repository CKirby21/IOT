// LED definitions in the datasheet 
// D13 (PIN_LED_13): Blue  
// TX (PIN_LED_TXL): Green  
// RX (PIN_LED_RXL): Yellow 

// Sparkfun SAMD21 ProRF using Atmel-ICE 

void setup() {
  // put your setup code here, to run once
  SerialUSB.begin(9600); 
//  while (!SerialUSB);
  pinMode(PIN_LED_13, OUTPUT);
  digitalWrite(PIN_LED_13, HIGH);

  // Generic clock generator 2, divisor = 32 (2^(DIV+1))
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(3); // 3 gives a divisor of 16
  // Enable clock generator 2 using low-power 32KHz oscillator.
  // With /16 divisor above, this yields 2048Hz(ish) clock.
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
  GCLK_GENCTRL_GENEN |
  GCLK_GENCTRL_SRC_OSCULP32K |
  GCLK_GENCTRL_DIVSEL;
  while(GCLK->STATUS.bit.SYNCBUSY); // Think about why this is used
  // WDT clock = clock gen 2
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
  GCLK_CLKCTRL_CLKEN |
  GCLK_CLKCTRL_GEN_GCLK2;
  
  WDT->CTRL.reg = 0; // Disable watchdog for config
  
  WDT->CONFIG.bit.PER = setWatchdog(4000); // Set period for chip reset from the datasheet
  WDT->INTENCLR.bit.EW = 1; // Disable early warning interrupt
  WDT->CTRL.bit.WEN = 0; // Disable window mode
  
  WDT->CTRL.bit.ENABLE = 1;
 
}

void loop() {
  // put your main code here, to run repeatedly:
}

int setWatchdog(int period){

  // Calculate how many cycles a certain period corresponds to
  const int clock_frequency_hz = 2048;
  const float period_seconds = period / 1000.0;
  const int cycles = period_seconds * clock_frequency_hz;

  // Set upper bounds for register cycles and value
  int register_cycles = 16384;
  int register_value = 0xB;

  while (register_value) {

    if (register_cycles <= cycles) 
      return register_value;
  
    register_cycles /= 2;
    register_value -= 0x1;
  }
  return 0x0;
}
