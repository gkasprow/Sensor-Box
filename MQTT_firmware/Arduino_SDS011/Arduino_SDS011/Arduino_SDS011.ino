//******************************
//*Abstract: Read value of PM1,PM2.5 and PM10 of air quality
//*Product Link: http://www.dfrobot.com.cn/goods-1113.html
//*
//8
//*The RX pin on the sensor connects to pin 10 on the Arduino
//*The TX pin on the sensor connects to pin 11 on the Arduino
//
//*Version：V3.1
//*Author：Zuyang @ HUST
//*Date：March.25.2016
//******************************


#include "credentials.h"

#define CAYENNE_PRINT SerialUSB  // Comment this out to disable prints and save space
#include <CayenneMQTTEthernetW5500.h>
//#include <Arduino.h>
//#include <SoftwareSerial.h>
#define LENG 9  //0xAA + 8 bytes s
unsigned char buf[LENG];




int PM2_5Value = 0, PM2_5_AV_Value = 0;    //define PM2.5 value of the air detector module
int PM10Value = 0, PM10_AV_Value = 0;    //define PM10 value of the air detector module
int AV_CNT = 0;

//SoftwareSerial PMSerial(PD1, PD0); // RX, TX

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(1500);
  SerialUSB.begin(9600);
//      while (!SerialUSB) {
//      ; // wait for serial port to connect. Needed for native USB port only
//    }
  SerialUSB.print("serial demo ");
  Cayenne.begin(username, password, clientID);
}


//void loop() { // run over and over
//  if (Serial1.available()) {
//    //Serial.print("got it ");
//    Serial.write(Serial1.read());
//  }
//}

//void test(){
void loop()
{
  Cayenne.loop();
  if (Serial.available())
  {
    if (Serial.read() == 0xAA)
    {
      Serial.readBytes(buf, LENG);
      if (buf[0] == 0xC0) {
        if (checkValue(buf, LENG)) {
          PM2_5Value = transmitPM2_5(buf); //count PM2.5 value of the air detector module
          PM10Value = transmitPM10(buf); //count PM10 value of the air detector module
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

    //   Serial.print("sum OK");
  }
  return receiveflag;
}


//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val = ((thebuf[2] << 8) + thebuf[1]); //count PM2.5 value of the air detector module
  return PM2_5Val;
}

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[4] << 8) + thebuf[3]); //count PM10 value of the air detector module
  return PM10Val;
}
