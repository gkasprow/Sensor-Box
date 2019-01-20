#include "credentials.h"
//pin mapping definition

#define D0      16
#define D1      5
#define D2      4
#define D3      0
#define D4      2
#define D5      14
#define D6      12
#define D7      13
#define D8      15



#include <ModbusMaster.h>
/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE      D1
#define MAX485_RE_NEG  D2

// instantiate ModbusMaster object as slave ID 2
// defaults to serial port 0 since no port was specified
ModbusMaster node;



// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneMQTTESP8266.h>



unsigned long lastMillis = 0;


// Virtual Pin of the DS18B20 widget.
#define TEMP_PIN V2
#define VIRTUAL_PIN V1

// Digital pin the DS18B20 is connected to. Do not use digital pins 0 or 1 since those conflict with the use of Serial.
const int tmpPin = D5;

OneWire oneWire(tmpPin);
DallasTemperature sensors(&oneWire);


void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
	Serial.begin(9600);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  
  // communicate with Modbus slave ID 1 over Serial (port 0)
  node.begin(1, Serial);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
//relay init
    pinMode(D8, OUTPUT);
    digitalWrite(D8, 0);

}





bool state = true;

void loop() {
	Cayenne.loop();





  
	//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
	if (millis() - lastMillis > 10000) {
		lastMillis = millis();
		//Write data to Cayenne here. This example just sends the current uptime in milliseconds.
		Cayenne.virtualWrite(0, lastMillis);
		//Some examples of other functions you can use to send data.
    sensors.requestTemperatures();
	//  Cayenne.celsiusWrite(TEMP_PIN, sensors.getTempCByIndex(0));
     Cayenne.virtualWrite(2, 25.5, "temp", "c");//temp
		//Cayenne.luxWrite(2, 700);
	
  uint8_t result,j;
  uint16_t data[6];
  // Read 2 registers starting at 0x0000)
 // result = node.readCoils(1,1);

 Cayenne.virtualWrite(13, digitalRead(D8), "digital_sensor", "Digital (0/1)");//relay ACK
 
    result = node.readHoldingRegisters(0, 9);
 if (result == node.ku8MBSuccess)
   {
    Cayenne.virtualWrite(4, (float)node.getResponseBuffer(0x00)/10,"voltage", "Volts"); //voltage
    Cayenne.virtualWrite(5, (float)node.getResponseBuffer(0x01)/10, "current", "Ampere");//current
    Cayenne.virtualWrite(6, (float)node.getResponseBuffer(0x02)/10, "freq", "Hertz" );//frequency
    Cayenne.virtualWrite(7, node.getResponseBuffer(0x03), "pow", "Watts");//Active power
    Cayenne.virtualWrite(8, node.getResponseBuffer(0x04), "pow", "Watts");//Reactive power
    Cayenne.virtualWrite(9, node.getResponseBuffer(0x05), "pow", "Watts");//Apparent power
    Cayenne.virtualWrite(10, (float)node.getResponseBuffer(0x06)/1000);//Power factor
    Cayenne.virtualWrite(11, (float)(node.getResponseBuffer(0x07) *256 + node.getResponseBuffer(0x08))/1000);// Active energy
    Cayenne.virtualWrite(12, node.getResponseBuffer(0x03), "pow", "Watts");//Active power
     
       
    //Serial.print("buffer: ");

     //   for (j = 0; j < 6; j++)
   // {
   //    Serial.println(node.getResponseBuffer(j));
   // }

     }
  
	}



 
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN(1)
{
  // get value sent from dashboard
  int currentValue = getValue.asInt(); // 0 to 1
//  Serial.println("MQTT Rx4"); 

  // assuming you wire your relay as normally open
  if (currentValue == 0) {
//    node.writeSingleCoil(0x0000, 0);
  Serial.println("MQTT Rx4 OFF"); 
  } else {
//    node.writeSingleCoil(0x0000, 1);
  Serial.println("MQTT Rx4 ON"); 
  }
}


CAYENNE_IN(2)
{
  // get value sent from dashboard
  int currentValue = getValue.asInt(); // 0 to 1
 // Serial.println("MQTT Rx2"); 
    // assuming you wire your relay as normally open
  if (currentValue == 0) {
    digitalWrite(D8, 0);
  } else {
    digitalWrite(D8, 1);
  }
}

CAYENNE_IN(3)
{
  // get value sent from dashboard
//analogWrite(D8, getValue.asInt() * 102);
}





/*
//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
   Serial.println("MQTT Rx"); 
}
*/
