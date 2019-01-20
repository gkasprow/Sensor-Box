#include "credentials.h"

// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneMQTTESP8266.h>



unsigned long lastMillis = 0;




// Digital pin the DS18B20 is connected to. Do not use digital pins 0 or 1 since those conflict with the use of Serial.
const int tmpPin = D5;

OneWire oneWire(tmpPin);
DallasTemperature sensors(&oneWire);


void setup() {
	Serial.begin(9600);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);
}

void loop() {
	Cayenne.loop();

	//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
	if (millis() - lastMillis > 10000) {
		lastMillis = millis();
		//Write data to Cayenne here. This example just sends the current uptime in milliseconds.
		Cayenne.virtualWrite(0, lastMillis);
		//Some examples of other functions you can use to send data.
    sensors.requestTemperatures();
	  Cayenne.celsiusWrite(1, sensors.getTempCByIndex(0));
    Cayenne.celsiusWrite(2, sensors.getTempCByIndex(1));
    Cayenne.celsiusWrite(3, sensors.getTempCByIndex(2));
    Cayenne.celsiusWrite(4, sensors.getTempCByIndex(3));

      Serial.print("tem0: ");Serial.println(sensors.getTempCByIndex(0));
      Serial.print("tem1: ");Serial.println(sensors.getTempCByIndex(1));
      Serial.print("tem2: ");Serial.println(sensors.getTempCByIndex(2));
      Serial.print("tem3: ");Serial.println(sensors.getTempCByIndex(3));
		//Cayenne.luxWrite(2, 700);
		//Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
	}
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
	//Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
