/*
Cayenne Generic Digital Output Example

This sketch shows how to set up a Generic Digital Output with Cayenne.

The Cayenne Library is required to run this sketch. If you have not already done so you can install it from the Arduino IDE Library Manager.

Steps:
1. In the Cayenne Dashboard add a new Generic Digital Output.
2. Select a digital pin number. Do not use digital pins 0 or 1 since those conflict with the use of Serial.
3. Attach a digital output device to the digital pin on your Arduino matching the selected pin.
4. Set the token variable to match the Arduino token from the Dashboard.
5. Compile and upload this sketch.

Notice that there isn't much coding involved to interact with the digital pins.
Most of it is handled automatically from the Cayenne library.
*/
#define CAYENNE_DEBUG  
#define CAYENNE_PRINT Serial // Comment this out to disable prints and save space
//#include <CayenneEthernet.h>
#include <CayenneEthernetW5500.h>


// Cayenne authentication token. This should be obtained from the Cayenne Dashboard.
char token[] = "0wgy8ufxr3";
unsigned long lastMillis = 0;

void setup()
{
	Serial.begin(9600);
  Serial.print("init ");
	Cayenne.begin(token);
  Serial.print("starting ");
}

void loop()
{
	Cayenne.run();

//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
   // Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);
 //  Some examples of other functions you can use to send data.
    Cayenne.celsiusWrite(1, 22.0);
    Cayenne.luxWrite(2, 700);
//    Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  }

}

