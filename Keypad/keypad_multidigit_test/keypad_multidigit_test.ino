#include <Keypad.h>

/*
 * Setup Keypad with empty pin connected to GND on the arduino
 * Connect a resistor and a led in series with the D10 pin and GND
*/
const byte ROWS = 4; 
const byte COLS = 4; 
const int LED_PIN = 10;
char current_digit = '0';
char last_digit = '0';


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad myKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);//enable serial monitor
  pinMode(LED_PIN, OUTPUT);
}
 
void loop() {
  char myKey = myKeypad.getKey();
  if (myKey) {

      if (myKey >= '0' && myKey <= '9') {
        last_digit = current_digit;
        current_digit = myKey;
      }      
      else if (myKey == 'D') {
          int num_blinks = (current_digit - '0') + (last_digit - '0') * 10;
          for (int i=0; i<num_blinks; i++)
              blink(LED_PIN, 200);
              
          current_digit = '0';
          last_digit = '0';
      }
  }
}

void blink(int pin, int delay_ms) {
  digitalWrite(pin,HIGH);
  delay(delay_ms);
  digitalWrite(pin,LOW);
  delay(delay_ms);
}
