const int UPPER_PIN = 11;
const int LOWER_PIN = 3;
const int MIDDLE_PIN = LOWER_PIN + ((UPPER_PIN - LOWER_PIN) / 2);
const int BUTTON_PIN = 2; // Only pins 2 and 3 on the Nano can be used for interrupts
const int UPPER_DELAY = 80;
volatile int current_delay = UPPER_DELAY;
volatile int current_direction = 1;
int current_pin = LOWER_PIN;

void setup() {
  for (int pin=LOWER_PIN; pin<=UPPER_PIN; pin++) 
    pinMode(pin, OUTPUT);

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr, RISING);
}

void loop() {
  // Reset the delay if it gets below zero
  if (current_delay <= 0) 
    current_delay = UPPER_DELAY;

  // Flip when hitting the edge
  if (current_pin > UPPER_PIN) {
    current_pin = UPPER_PIN - 1;
    flip(1);
  }
  else if (current_pin < LOWER_PIN) {
    current_pin = LOWER_PIN + 1;
    flip(1);
  }

  blink(current_pin, current_delay);
  current_pin += current_direction;
}

void isr() {
  if (current_pin == MIDDLE_PIN) 
    flip(5);
  else 
    flip(1);
}

void blink(int pin, int delay_ms) {
  digitalWrite(pin,HIGH);
  delay(delay_ms);
  digitalWrite(pin,LOW);
  delay(delay_ms);
}

void flip(int delay_ms) {
  current_direction *= -1;
  current_delay -= delay_ms;
}
