/*
PIR HCSR501 
modified on 5 Feb 2019
by Saeed Hosseini @ ElectroPeak
https://electropeak.com/learn/guides/
*/
//const int pirPin = 10;                 // PIR Out pin 
//const int buzzerPin = 2;
const int PIR_PIN = 2;

void setup() { 
// pinMode(pirPin, INPUT);     
// pinMode(buzzerPin, OUTPUT);     
// Serial.begin(9600);
// delay(10000);
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
}
void loop(){
//  static int gots = 0;
//  static int pirStat = 0;
//  
//  pirStat = digitalRead(pirPin); 
//  if (pirStat == HIGH) {            // if motion detected
//    gots++;
//    Serial.print("Gots: ");
//    Serial.println(gots);
//    tone(buzzerPin, 440);
//    delay(1000);
//  } 
//  else {
//    noTone(buzzerPin);
//    Serial.print("Gots: ");
//    Serial.println(gots);
//  }
  if (digitalRead(PIR_PIN)) {
    Serial.println("Gotcha");
  }
}
