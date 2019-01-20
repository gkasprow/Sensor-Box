#include "credentials.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE           DHT22     // DHT 22 (AM2302)
#define DHTPIN           8         // Pin which is connected to the DHT sensor.
DHT_Unified dht(DHTPIN, DHTTYPE);
DHT dht1(DHTPIN, DHTTYPE);
uint32_t delayMS;


/*
Cayenne Relay Switch Example

This sketch sample file shows how to connect a Relay Switch.

The Cayenne Library is required to run this sketch. If you have not already done so you can install it from the Arduino IDE Library Manager.

Steps:
1. In the Cayenne Dashboard add a new Custom Widget, and select Button.
2. Select a Virtual pin number.
3. Set VIRTUAL_PIN to the pin number you selected.
4. Connect the Relay Switch to a digital pin.
5. Set RELAY_DIGITAL_PIN to the digital pin number you selected.
6. Set the token variable to match the Arduino token from the Dashboard.
7. Compile and upload this sketch.
8. Once the Arduino connects to the Dashboard use the button widget to
   turn on/off an LED wired through the Relay.
*/


#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space

// If you're not using the Ethernet W5100 shield, change this to match your connection type. See Communications examples.
#include <CayenneEthernet.h>

#define VIRTUAL_PIN 1



unsigned long lastMillis = 0;

void setup()
{
  Serial.begin(9600); 
  Serial1.begin(9600); 
  // Initialize device.
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);

  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  
  /*  pinMode(PD2, OUTPUT);
    digitalWrite(PD2, 1);

  
     pinMode(PD3, OUTPUT);
    digitalWrite(PD3, 1);
    */
  Cayenne.begin(token);

   
}

/*void loopq() {
  // read from port 1, send to port 0:
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
}
*/

void loop()
{
  Cayenne.run();

  
    Serial1.print("Humidity: ");
//    Serial1.print(h);
    Serial1.print(" %, Temp: ");
 //   Serial1.print(t);
    Serial1.println(" Celsius");
    delay(200); //Delay 2 sec.
    

//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
      lastMillis = millis();

        sensors_event_t event;  
 

    /// dht.temperature().getEvent(&event);
    float t = dht1.readTemperature();
  // Cayenne.celsiusWrite(4, event.temperature);
  
   //  dht.humidity().getEvent(&event);
  // Cayenne.virtualWrite(3,event.relative_humidity, HUMIDITY, "null");
    float h = dht1.readHumidity();

 
    if (!isnan(h) && !isnan(t)) {
      Cayenne.celsiusWrite(4, t);
    Cayenne.virtualWrite(3,h);
    }

    
  }

  
  
}


// This function is called when the Cayenne widget requests data for the Virtual Pin.
CAYENNE_OUT(3)
{
  // Read data from the sensor and send it to the virtual channel here.
  // You can write data using virtualWrite or other Cayenne write functions.
  // For example, to send a temperature in Celsius you can use the following:
  // Cayenne.virtualWrite(VIRTUAL_PIN, 25.5, TEMPERATURE, CELSIUS);


  
}

