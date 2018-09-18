/*
  LoRa Simple Yun Server :
  Support Devices: LG01. 
  
  Example sketch showing how to create a simple messageing server, 
  with the RH_RF95 class. RH_RF95 class does not provide for addressing or
  reliability, so you should only use RH_RF95 if you do not need the higher
  level messaging abilities.

  It is designed to work with the other example LoRa Simple Client

  User need to use the modified RadioHead library from:
  https://github.com/dragino/RadioHead

  modified 16 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/
//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01. 
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines. 
//#define BAUDRATE 250000

//#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

int state=0;
int hum=0;
int temp = 0;
int led = A2;
int tick=0;
float frequency = 915.0;

void setup() 
{
  pinMode(led, OUTPUT);     
 // Bridge.begin(BAUDRATE);
 // Console.begin();
 // while (!Console) ; // Wait for console port to be available
 // Console.println("Start LoRa Sketch");
  if (!rf95.init())
  //  Console.println("init failed");
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
  
 // Console.print("Listening on frequency: ");
 // Console.println(frequency);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len);
    //  Console.print("got request: ");
    //  Console.println((char*)buf);
    //  Console.print("RSSI: ");
    //  Console.println(rf95.lastRssi(), DEC);

    hum = (buf[0] <<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3] ;
    temp = (buf[4] <<24) + (buf[5]<<16) + (buf[6]<<8) + buf[7] ;
    
      delay(100); 
      // Send a reply
      //uint8_t data[] = "And hello back to you";
      //rf95.send(buf, len);
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
tick++;
if (tick == 1000000) 
            {
  tick=0;
 // Console.print(".");
 // digitalWrite(led, (state) ? HIGH : LOW);
 // state = !state;
//  digitalWrite(led, TOGGLE);   // turn the HEART_LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(led, LOW);    // turn the HEART_LED off by making the voltage LOW
//  delay(1000);              // wait for a second
            }
  }

  
}


