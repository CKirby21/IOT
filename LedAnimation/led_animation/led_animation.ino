const int UPPER_PIN = 9;
const int LOWER_PIN = 4;
const int BUTTON_PIN = 2; // Only pins 2 and 3 on the Nano can be used for interrupts
int current_pin = LOWER_PIN;
volatile int current_direction = 1;

volatile int button_state = 0;

void setup() {
  for (int pin=LOWER_PIN; pin<=UPPER_PIN; pin++) 
    pinMode(pin, OUTPUT);

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, RISING);
}

void blink(int pin, int delay_ms) {
  digitalWrite(pin,HIGH);
  delay(delay_ms);
  digitalWrite(pin,LOW);
  delay(delay_ms);
}

void loop() {
  blink(current_pin, 50);
  current_pin += current_direction;
  if (current_pin >= UPPER_PIN) { || current_pin <= LOWER_PIN) 
    current_direction *= -1;
}

void buttonPressed() {
  current_direction *= -1;
}
