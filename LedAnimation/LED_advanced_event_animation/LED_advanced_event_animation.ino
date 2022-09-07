const int UPPER_PIN = 11;
const int LOWER_PIN = 3;
const int GOAL_PIN = LOWER_PIN + ((UPPER_PIN - LOWER_PIN) / 2);
const int BUTTON_PIN = 12; // Only pins 2 and 3 on the Nano can be used for interrupts
const int BUZZER_PIN = 2;
const int UPPER_DELAY = 80;
int current_delay = UPPER_DELAY;
int current_direction = 1;
int current_pin = LOWER_PIN;
volatile int current_button_state = LOW;
int last_button_state = LOW;

void setup() {
//  Serial.begin(9600);
  
  for (int pin=LOWER_PIN; pin<=UPPER_PIN; pin++) 
    pinMode(pin, OUTPUT);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
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

  current_button_state = digitalRead(BUTTON_PIN);
  if (current_button_state == HIGH && last_button_state == LOW && current_pin == GOAL_PIN) {
    blink(current_pin, UPPER_DELAY);
    blink(BUZZER_PIN, UPPER_DELAY);
    current_delay -= 3;
  }

  last_button_state = current_button_state;
  current_pin += current_direction;
}

void flip(int delay_ms) {
  current_direction *= -1;
  current_delay -= delay_ms;
}

void blink(int pin, int delay_ms) {
  digitalWrite(pin,HIGH);
  delay(delay_ms);
  digitalWrite(pin,LOW);
  delay(delay_ms);
}
