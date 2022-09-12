/*
  Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" pie
ce of wire.
  This example is a modified version of the example provided by the Radio Head
  Library which can be found here:
  www.github.com/PaulStoffregen/RadioHeadd
*/

#include <SPI.h>

//Radio Head Library:
#include <RH_RF95.h>

#include <TemperatureZero.h>

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED is on pin 13

int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received

TemperatureZero TempZero = TemperatureZero();

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
float frequency = 906; //Broadcast frequency

void setup()
{
  pinMode(LED, OUTPUT);

  SerialUSB.begin(9600);
  TempZero.init();
  // It may be difficult to read serial messages on startup. The following line
  // will wait for serial to be ready before continuing. Comment out if not needed.
//  while (!SerialUSB);
  SerialUSB.println("RFM Client!");

  //Initialize the Radio.
  if (rf95.init() == false) {
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  }
  else {
    //An LED inidicator to let us know radio initialization has completed.
//    rf95.setModemConfig(Bw125Cr48Sf4096); // slow and reliable?
    SerialUSB.println("Transmitter up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  // Set frequency
    rf95.setFrequency(frequency);

  // Transmitter power can range from 14-20dbm.
  rf95.setTxPower(20, false);
}


void loop()
{
//  SerialUSB.println("Sending message");
  // Send a message to the other radio
//  char toSend[] = "pew";
//  packetCounter = packetCounter++;
//  SerialUSB.println(toSend);
//  SerialUSB.println(packetCounter);

  float avg;

  for (int i = 0; i < 5; i++) {
    float temperature = TempZero.readInternalTemperature();
    avg += temperature;
    delay(1000);
  }

  avg /= 5;
  char toSend[100];
  gcvt(avg, 6, toSend);
  SerialUSB.print("Internal Temperature is:");
  SerialUSB.println(avg);

  rf95.send((uint8_t *)toSend, sizeof(toSend));

//  rf95.send((uint8_t *)toSend, sizeof(toSend));
//  rf95.waitPacketSent();
//  delay(1000);


}