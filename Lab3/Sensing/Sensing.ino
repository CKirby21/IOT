/*
  Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" pie
ce of wire.
  This example is a modified version of the example provided by the Radio Head
  Library which can be found here:
  www.github.com/PaulStoffregen/RadioHeadd
*/

#include <SPI.h>
#include <RH_RF95.h>
#include <TemperatureZero.h>

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED is on pin 13
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received


// Define Packet
const int nodeID = 0; // CHANGE for your node
int packetID = 0;
unsigned long timestamp = 0;

TemperatureZero TempZero = TemperatureZero();

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
float frequency = 906; //Broadcast frequency

void setup()
{
  pinMode(LED, OUTPUT);

  SerialUSB.begin(9600);
  TempZero.init();
  while (!SerialUSB);
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

  SerialUSB.print("NodeID\t");
  SerialUSB.println(nodeID);

  float avg;
  for (int i = 0; i < 5; i++) {
    float temperature = TempZero.readInternalTemperature();
    avg += temperature;
    delay(1000);
  }
  avg /= 5;
  SerialUSB.print("Temperature\t");
  SerialUSB.println(avg);

  timestamp = millis();
  SerialUSB.print("Timestamp\t");
  SerialUSB.println(timestamp);

  SerialUSB.print("PacketID\t");
  SerialUSB.println(packetID);

  char packet[100];
  sprintf(packet, "%d,%d,%lu,%f", nodeID, packetID, timestamp, avg);
  SerialUSB.print("Packet\t");
  SerialUSB.println(packet);
  
  rf95.send((uint8_t *)packet, sizeof(packet));
  rf95.waitPacketSent();
  packetID++;

  SerialUSB.println();
}
