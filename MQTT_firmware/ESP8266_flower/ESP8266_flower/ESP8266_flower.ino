#include "credentials.h"
#define CAYENNE_DEBUG

/*
    This sketch shows the Ethernet event usage

*/

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define CAYENNE_LOG Serial
#include <CayenneMQTTESP8266.h>

/* create an instance of PubSubClient client */
WiFiClient espClient;

int inputVal  = 0;






unsigned long lastMillis = 0;

const int analog_ip = A0; //Naming analog input pin



void setup()
{
   
   
  Serial.setTimeout(1500);    
  
  Serial.begin(115200);
  //WiFi.onEvent(WiFiEvent);
  Serial.print("Connecting to ");
  Serial.println(ssid);

 // ETH.begin();

Cayenne.begin(username, password, clientID, ssid, wifiPassword);
Serial.print("Cayenne CFG done ");

}


void loop()
{
    Cayenne.loop();

  
  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
  //  Cayenne.virtualWrite(0, lastMillis);
    //Some examples of other functions you can use to send data.
        Serial.print("\nHumidity: ");
     Serial.println(analogRead (analog_ip));

 inputVal =0;
   for (int i=0; i<10; i++)
   {
   inputVal += analogRead (analog_ip); // Analog Values 0 to 1023
   }
   
      Serial.print("\nHumidity: ");
     Serial.println(inputVal);
    Cayenne.virtualWrite(0, inputVal/94, "soil_moist", "p");
  }


    

     
}
