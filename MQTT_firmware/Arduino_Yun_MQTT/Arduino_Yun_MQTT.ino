// This example shows how to connect to Cayenne using an Arduino Yun and send/receive sample data.

//#define CAYENNE_DEBUG
//#define CAYENNE_PRINT Serial
#include <CayenneMQTTYun.h>
#include <SPI.h>
#include <RH_RF95.h>
#define VIRTUAL_CHANNEL 10
// Singleton instance of the radio driver
RH_RF95 rf95;

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e";
char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
char clientID[] = "59ea40b0-a39c-11e8-b90b-23f7758e03a0";

unsigned long lastMillis = 0;
float frequency = 915.0;
int led = A2;
int hum=0;
int temp = 0;

void setup() {
 //   Serial.begin(250000);
    pinMode(led, OUTPUT); 
 //   while (!Serial) ; // Wait for console port to be available
 // Serial.println("Start Sketch");
  if (!rf95.init())
//    Serial.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);
  
  // Setup Spreading Factor (6 ~ 12)
  rf95.setSpreadingFactor(7);
  
  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setSignalBandwidth(125000);
  
  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
  rf95.setCodingRate4(5);
  
 // Serial.print("Listening on frequency: ");
 // Serial.println(frequency);
  Cayenne.begin(username, password, clientID);
  
}

void loop() {

  Cayenne.loop();
 

  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
   Cayenne.virtualWrite(0, lastMillis);
    //Some examples of other functions you can use to send data.
    //Cayenne.celsiusWrite(1, 22.0);
    //Cayenne.luxWrite(2, 700);
    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
     Cayenne.virtualWrite(1, (float) hum /100);
     Cayenne.virtualWrite(2, (float) temp /100);
  }

 if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len);

    hum = (buf[0] <<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3] ;
    temp = (buf[4] <<24) + (buf[5]<<16) + (buf[6]<<8) + buf[7] ;

    //  delay(10); 

      rf95.send(buf, sizeof(buf));
      rf95.waitPacketSent();
     // Console.println("Sent a reply");
      digitalWrite(led, LOW);
    }
    else
    {
   //   Console.println("recv failed");
    }
  }
      else
    {
      digitalWrite(led, LOW);
    }

//
//if (rf95.available())
//  {
//    digitalWrite(led, HIGH);
//    // Should be a message for us now   
//    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//    uint8_t len = sizeof(buf);
//    if (rf95.recv(buf, &len))
//    {
//      
//      RH_RF95::printBuffer("request: ", buf, len);
//      digitalWrite(led, LOW);
//      digitalWrite(led, HIGH);
//    //  Console.print("got request: ");
//    //  Console.println((char*)buf);
//    //  Console.print("RSSI: ");
//    //  Console.println(rf95.lastRssi(), DEC);
//    //  delay(100); 
//      // Send a reply
//      uint8_t data[] = "And hello back to you";
//      rf95.send(data, sizeof(data));
//      
//       digitalWrite(led, LOW);
//      digitalWrite(led, HIGH);
//      rf95.waitPacketSent();
//       digitalWrite(led, LOW);
//      digitalWrite(led, HIGH);
//     // Console.println("Sent a reply");
//      // Cayenne.virtualWrite(1, lastMillis);
//       digitalWrite(led, LOW);
//    }
//    else
//    {
//      
//   //   Console.println("recv failed");
//    }
//    
//  }
//      else
//    {
//      digitalWrite(led, LOW);
//    }

  
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}

 


