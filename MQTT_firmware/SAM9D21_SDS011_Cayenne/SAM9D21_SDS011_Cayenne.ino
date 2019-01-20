#include "credentials.h"
  /*
This example shows how to connect to Cayenne using an Ethernet W5500 shield and send/receive sample data.

The CayenneMQTT Library is required to run this sketch. If you have not already done so you can install it from the Arduino IDE Library Manager.

Steps:
1. Download the Ethernet2 library (https://github.com/adafruit/Ethernet2) as a zip file.
2. From the Arduino IDE Include Library menu select Add .ZIP Library and add the downloaded Ethernet2 zip library.
3. Set the Cayenne authentication info to match the authentication info from the Dashboard.
4. Compile and upload the sketch.
5. A temporary widget will be automatically generated in the Cayenne Dashboard. To make the widget permanent click the plus sign on the widget.
*/


#define CAYENNE_LOG SerialUSB
#define CAYENNE_PRINT SerialUSB  // Comment this out to disable prints and save space
#include <CayenneMQTTEthernetW5500.h>

#define LENG 9  //0xAA + 8 bytes s
unsigned char buf[LENG];
int PM2_5Value = 0, PM2_5_AV_Value = 0;    //define PM2.5 value of the air detector module
int PM10Value = 0, PM10_AV_Value = 0;    //define PM10 value of the air detector module
int AV_CNT = 0;



// Mac address should be different for each device in your LAN
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
unsigned long lastMillis = 0;

void setup() {
	SerialUSB.begin(9600);
 SerialUSB.print("init ");
 
  Serial1.begin(9600);
  Serial1.setTimeout(1500);
 
 delay(100);
	Cayenne.begin(username, password, clientID, arduino_mac );
  SerialUSB.print("starting ");
  
}

void loop()
{
  Cayenne.loop();
  if (Serial1.available())
   SerialUSB.print("A ");
  {
    if (Serial1.read() == 0xAA)
    SerialUSB.print("AA ");
    {
      Serial1.readBytes(buf, LENG);
      SerialUSB.print("R ");
      if (buf[0] == 0xC0) {
        SerialUSB.print("C0 ");
        if (checkValue(buf, LENG)) {
          SerialUSB.print("val ");
          PM2_5Value = ((buf[2] << 8) + buf[1]); //count PM2.5 value of the air detector module
          PM10Value = ((buf[4] << 8) + buf[3]);; //count PM10 value of the air detector module
          
        }
      }
    }
  }

  static unsigned long OledTimer = millis();
  if (millis() - OledTimer >= 1000)
  {
    OledTimer = millis();

    SerialUSB.print("PM2.5: ");
    SerialUSB.print(PM2_5Value);
    SerialUSB.println("  ug/m3");

    SerialUSB.print("PM10: ");
    SerialUSB.print(PM10Value);
    SerialUSB.println("  ug/m3");
    SerialUSB.println();


    if (AV_CNT++ < 10)
    {
      PM2_5_AV_Value += PM2_5Value;
      PM10_AV_Value += PM10Value;
    }
    else
    {
      AV_CNT = 0;
//      Serial.print("PM2.5 AV: ");
//      Serial.print(PM2_5_AV_Value);
//      Serial.println("  ug/m3");
//
//      Serial.print("PM10 AV: ");
//      Serial.print(PM10_AV_Value);
//      Serial.println("  ug/m3");
//      Serial.println();
      Cayenne.virtualWrite(3, (float)(PM2_5_AV_Value / 100));
      Cayenne.virtualWrite(4, (float)(PM10_AV_Value / 100));
      PM2_5_AV_Value = 0;
      PM10_AV_Value = 0;
    }
  }

}
char checkValue(unsigned char *thebuf, char leng)
{
  char receiveflag = 0;
  int receiveSum = 0;

  //   for(int i=0; i<(LENG); i++){
  //            Serial.print(buf[i], HEX);
  //             }


  for (int i = 1; i < (leng - 2); i++) {
    receiveSum = receiveSum + thebuf[i];
  }
  receiveSum = receiveSum & 0xFF;
  //   Serial.print("sum");
  //  Serial.print(receiveSum, HEX);
  //   Serial.print(thebuf[leng-1],HEX);

  if (receiveSum == thebuf[leng - 2]) //check the serial data
  {
    receiveSum = 0;
    receiveflag = 1;

       SerialUSB.print("sum OK");
  }
  return receiveflag;
}
