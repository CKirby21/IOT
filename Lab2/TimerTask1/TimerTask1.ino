// LED definitions in the datasheet
// D13 (PIN_LED_13): Blue
// TX (PIN_LED_TXL): Green
// RX (PIN_LED_RXL): Yellow

void setup() {
    SerialUSB.begin(9600);
    while(!SerialUSB);
    pinMode(PIN_LED_13, OUTPUT);
    pinMode(PIN_LED_RXL, OUTPUT);
    pinMode(PIN_LED_RXL, HIGH);
}

void loop()
{
    static long previousBlueMillis = 0;
    static long previousYellowMillis = 0;
    static bool isBlueOn = false;
    static bool isYellowOn = false;
    static const long blueInterval = 500;
    static const long yellowInterval = 1000;
    
    unsigned long currentMillis = millis();
    if(currentMillis - previousBlueMillis > blueInterval) {
        // save the last time you blinked the LED
        previousBlueMillis = currentMillis;
        isBlueOn = !isBlueOn;
        digitalWrite(PIN_LED_13, isBlueOn);

        if (isBlueOn)
          SerialUSB.println("Blue LED is on");
        else
          SerialUSB.println("Blue LED is off");
    } 
    if (currentMillis - previousYellowMillis > yellowInterval) {
        // save the last time you blinked the LED
        previousYellowMillis = currentMillis;
        digitalWrite(PIN_LED_RXL, isYellowOn);
        isYellowOn = !isYellowOn;

        if (isYellowOn)
          SerialUSB.println("Yellow LED is on");
        else 
          SerialUSB.println("Yellow LED is off");
    }
}
