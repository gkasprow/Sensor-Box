#include "credentials.h"
#include "PMS.h"

#define INFO_CPU ESP8266
#define CAYENNE_PRINT Serial
#define CAYENNE_LOG Serial
#include <CayenneMQTTESP8266.h>

PMS pms(Serial);
PMS::DATA data;

float PM2_5Value=0,PM2_5_AV_Value=0;         //define PM2.5 value of the air detector module
float PM10Value=0,PM10_AV_Value=0;         //define PM10 value of the air detector module
float PM1Value=0,PM1_AV_Value=0;         //define PM10 value of the air detector module
int AV_CNT=0,RSSI_AV=0;

unsigned long lastMillis = 0;




void setup()
{
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial.swap();
  Serial1.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)
  Serial.println("started");
  Serial1.println("started");
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
}

void loop()
{
 
  if (pms.read(data))
  {
    Cayenne.loop();
    Serial1.println("Data:");

    Serial1.print("PM 1.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_1_0);

    Serial1.print("PM 2.5 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_2_5);

    Serial1.print("PM 10.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_10_0);

    Serial1.println();

if (AV_CNT++<10)
  {
  PM1_AV_Value += data.PM_AE_UG_1_0;
  PM2_5_AV_Value += data.PM_AE_UG_2_5;
  PM10_AV_Value += data.PM_AE_UG_10_0;
  RSSI_AV += WiFi.RSSI();
  }
else
  {
    AV_CNT=0;
    
    

//      Serial.print("PM1 AV: ");  
//      Serial.print(PM1_AV_Value);
//      Serial.println("  ug/m3");   
//      
//      Serial.print("PM2.5 AV: ");  
//      Serial.print(PM2_5_AV_Value);
//      Serial.println("  ug/m3"); 
//            
//      Serial.print("PM10 AV: ");  
//      Serial.print(PM10_AV_Value);
//      Serial.println("  ug/m3");   
//      Serial.println();
      
      Cayenne.virtualWrite(0, lastMillis);
      Cayenne.virtualWrite(2,(float)(PM1_AV_Value/10));
      Cayenne.virtualWrite(3,(float)(PM2_5_AV_Value/10));
      Cayenne.virtualWrite(4,(float)(PM10_AV_Value/10));
      Cayenne.virtualWrite(5, (float)RSSI_AV/10, "snr", "db");
      RSSI_AV = 0;
      PM1_AV_Value = 0;
      PM2_5_AV_Value = 0;
      PM10_AV_Value = 0;
  }
  }

//  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
//  if (millis() - lastMillis > 10000) {
//    lastMillis = millis();
//    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
//    Cayenne.virtualWrite(0, lastMillis);
//      Cayenne.virtualWrite(2,(float)(PM1_AV_Value/10));
//      Cayenne.virtualWrite(3,(float)(PM2_5_AV_Value/10));
//      Cayenne.virtualWrite(4,(float)(PM10_AV_Value/10));
//    // Serial.print(".");
//    //Some examples of other functions you can use to send data.
//    //Cayenne.celsiusWrite(1, 22.0);
//    //Cayenne.luxWrite(2, 700);
//    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
//  }
  

  
   
}
