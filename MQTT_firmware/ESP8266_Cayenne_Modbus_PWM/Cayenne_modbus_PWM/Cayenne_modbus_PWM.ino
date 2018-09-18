
//pin mapping definition

#define D0      16
#define D1      5
#define D2      4
#define REL_O      0
#define REL_C      2
#define D5      14
#define D6      12
#define D7      13
#define D8      15



//#include <ModbusMaster.h>
/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
//#define MAX485_DE      D1
//#define MAX485_RE_NEG  D2

// instantiate ModbusMaster object as slave ID 2
// defaults to serial port 0 since no port was specified
//ModbusMaster node;



// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneMQTTESP8266.h>

// WiFi network info.
char ssid[] = "Casper";
char wifiPassword[] = "abbaabbaff.";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e";
char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
char clientID[] = "e4b31b10-368a-11e8-b6d4-35a0ad51f849";

unsigned long lastMillis = 0;


// Virtual Pin of the DS18B20 widget.
#define TEMP_PIN V2
#define VIRTUAL_PIN V1

// Digital pin the DS18B20 is connected to. Do not use digital pins 0 or 1 since those conflict with the use of Serial.
const int tmpPin = D5;
unsigned char fan_speed;


OneWire oneWire(tmpPin);
DallasTemperature sensors(&oneWire);


void setup() {

	Serial.begin(9600);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(REL_O, OUTPUT); 
  pinMode(REL_C, OUTPUT); 
  digitalWrite(REL_O, LOW);
  digitalWrite(REL_C, LOW);
  fan_speed = 10;
  analogWrite(D8, fan_speed*100); // set PWM to 10%
}





bool state = true;

void loop() {
	Cayenne.loop();

  
	//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
	if (millis() - lastMillis > 10000) {
		lastMillis = millis();
		//Write data to Cayenne here. This example just sends the current uptime in milliseconds.
	//	Cayenne.virtualWrite(0, lastMillis);
		//Some examples of other functions you can use to send data.
    sensors.requestTemperatures();
	  Cayenne.celsiusWrite(TEMP_PIN, sensors.getTempCByIndex(0));

	   Cayenne.virtualWrite(4, WiFi.RSSI(), "snr", "db");



  
	}



 
}





CAYENNE_IN(5)
{
  // get value sent from dashboard
        fan_speed = getValue.asInt();
    analogWrite(D8, fan_speed * 102);
    Cayenne.virtualWrite(3, fan_speed);
}
CAYENNE_IN(6)
{
  // get value sent from dashboard
  if (getValue.asInt())
  {
  digitalWrite(REL_O, HIGH);
  Cayenne.virtualWrite(6, digitalRead(REL_O));
  digitalWrite(REL_C, LOW);
    Cayenne.virtualWrite(8, digitalRead(REL_C));
  }
}

CAYENNE_IN(8)
{
    if (getValue.asInt())
  {
  // get value sent from dashboard
  digitalWrite(REL_O, LOW);
    Cayenne.virtualWrite(6, digitalRead(REL_O));
  digitalWrite(REL_C, HIGH);
      Cayenne.virtualWrite(8, digitalRead(REL_C));
  }
}



CAYENNE_OUT_DEFAULT()
{
 Cayenne.virtualWrite(7, digitalRead(REL_O));
  Cayenne.virtualWrite(8, digitalRead(REL_C));
}

