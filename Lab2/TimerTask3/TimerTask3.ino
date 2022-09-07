// LED definitions in the datasheet
// D13 (PIN_LED_13): Blue
// TX (PIN_LED_TXL): Green
// RX (PIN_LED_RXL): Yellow

bool isBlueOn = false;
bool isYellowOn = false;
int interruptCount = 0;

void setup() {
  
  pinMode(PIN_LED_13, OUTPUT);
  pinMode(PIN_LED_RXL, OUTPUT);
  digitalWrite(PIN_LED_RXL, HIGH);
  
   // Set up the generic clock (GCLK2) used to clock timers
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(4) |          // Divide the 48MHz clock source by divisor 3: 32zKHz/(2^(4+1))=1KHz
                    GCLK_GENDIV_ID(2);            // Select Generic Clock (GCLK) 2
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |        // Select GCLK2
                    GCLK_GENCTRL_GENEN |          // Enable GCLK2
                    GCLK_GENCTRL_SRC_OSCULP32K |  // Set the 32KHz clock source
                    GCLK_GENCTRL_DIVSEL;
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK2 to TC3 and TC2
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TC3 and TC2
                     GCLK_CLKCTRL_GEN_GCLK2 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC2_TC3;     // Feed the GCLK4 to TC3 and TC2
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_TC3_CTRLA |= TC_CTRLA_MODE_COUNT8;           // Set the counter to 8-bit mode
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization

  REG_TC3_COUNT8_PER = 0xFF;                      // Set the PER (period) register to its maximum value
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization

  NVIC_SetPriority(TC3_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TC3 to 0 (highest)
  NVIC_EnableIRQ(TC3_IRQn);         // Connect TC3 to Nested Vector Interrupt Controller (NVIC)

  REG_TC3_INTFLAG |= TC_INTFLAG_OVF;        // Clear the interrupt flags
  REG_TC3_INTENSET = TC_INTENSET_OVF;     // Enable TC3 interrupts
 
  REG_TC3_CTRLA |= TC_CTRLA_PRESCALER_DIV2 |     // Set prescaler to 2, 1KHz/2 = 512Hz
                   TC_CTRLA_ENABLE;               // Enable TC3
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
}

void loop() {
  // put your main code here, to run repeatedly:

}

void TC3_Handler()
{     
  // Check for overflow (OVF) interrupt
  if (TC3->COUNT8.INTFLAG.bit.OVF)             
  {
    isBlueOn = !isBlueOn;
    digitalWrite(PIN_LED_13, isBlueOn);
    if (interruptCount % 2 == 0) {
      digitalWrite(PIN_LED_RXL, isYellowOn);
      isYellowOn = !isYellowOn;
    }
    interruptCount++;
    REG_TC3_INTFLAG = TC_INTFLAG_OVF;         // Clear the OVF interrupt flag
  }
}
