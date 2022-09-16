
/*
 Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" pie
ce of wire.
 This example is a modified version of the example provided by the Radio Head
 Library which can be found here:
 www.github.com/PaulStoffregen/RadioHeadd
*/
#include <SPI.h>
#include <FlashStorage.h>

//Radio Head Library:
#include <RH_RF95.h>

// We need to provide the RFM95 module's chip select and interrupt pins to the
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

// Reserve a portion of flash memory to store an "int" variable
// and call it "my_flash_store".
FlashStorage(my_flash_store, int);

int LED = 13; //Status LED is on pin 13
int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received
int lastPacketIDs[4] = {0,0,0,0};

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 868MHz.
// This works but it is unknown how well the radio configures to this frequency:
float frequency = 906; //Broadcast frequency


void setup()
{
  pinMode(LED, OUTPUT);
  SerialUSB.begin(9600);
  // while (!SerialUSB);
  
  //Initialize the Radio.
  SerialUSB.println("RFM Client!");
  if (rf95.init() == false) {
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  }
  else {
    //An LED inidicator to let us know radio initialization has completed.
    SerialUSB.println("Receiver up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }
  rf95.setFrequency(frequency);
  // Transmitter power can range from 14-20dbm.
  rf95.setTxPower(20, false);
  uint8_t toSend[] = "Start";  
  rf95.send(toSend, sizeof(toSend));
  rf95.waitPacketSent();  
  rf95.setTxPower(20, true);
}


void loop()
{
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)){
      digitalWrite(LED, HIGH); //Turn on status LED
      timeSinceLastPacket = millis(); //Timestamp this packet
      SerialUSB.println((char*)buf);
  
      char *message = strtok((char *)buf, ",");
      int nodeID = atoi(message);
  
      message = strtok(NULL, ",");
      int packetID = atoi(message);
  
      message = strtok(NULL, ",");
      unsigned long timestamp = strtoul(message, NULL, 10);
  
      message = strtok(NULL, ",");
      float temperature = atof(message);
  
      message = strtok(NULL, ",");
      int errorCode = atoi(message);
  
      int idealPacketID = lastPacketIDs[nodeID] + 1;
      if (packetID > idealPacketID)
        errorCode = 2; 
      lastPacketIDs[nodeID] = packetID;
        
      // Log error and write it to flash
      if (errorCode > 0) {
        my_flash_store.write(errorCode);
        SerialUSB.print("Node ");
        SerialUSB.print(nodeID);
        SerialUSB.print(" had an error code of ");
        SerialUSB.print(my_flash_store.read());
        SerialUSB.print(" at time ");
        SerialUSB.println(timestamp);
      }
    } else
      SerialUSB.println("Recieve failed");
  }
  //Turn off status LED if we haven't received a packet after 1s
  if(millis() - timeSinceLastPacket > 1000){
    digitalWrite(LED, LOW); //Turn off status LED
    timeSinceLastPacket = millis(); //Don't write LED but every 1s
  }
}
