#include "credentials.h"
#include <Servo.h>



#define CAYENNE_PRINT Serial1
#define CAYENNE_LOG Serial1
#include <CayenneMQTTESP8266.h>

Servo myservo; 
int val;    // variable to read the value from the analog pin
unsigned long lastMillis = 0;




void setup()
{
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial.println("started");
  myservo.attach(D8);  // attaches the servo on pin 9 to the servo object
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
}

void loop()
{
  Cayenne.loop();

  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);

    // Serial.print(".");
    //Some examples of other functions you can use to send data.
    //Cayenne.celsiusWrite(1, 22.0);
    //Cayenne.luxWrite(2, 700);
    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  }
}

CAYENNE_IN(1)
{
  // get value sent from dashboard

val = map(getValue.asInt(), 0, 10, 0, 170); 
Serial.print(val);
Serial.println("");
myservo.write(val); 
}

