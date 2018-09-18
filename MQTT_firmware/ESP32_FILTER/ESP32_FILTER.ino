#include <Stepper.h>
#include <Wire.h>
#include "Arduino.h"
#include <Adafruit_SGP30.h>
#define SDA 13
#define SCL 15
#define EN_I2C1 26
#define EN_I2C2 27

#define CAYENNE_DEBUG

int current_pos = 0;
int desired_pos = 0;

//33    1   yellow    seq 3
//32    2   pink      seq 2
//vcc   3   red     
//35    4   orange    seq 4
//34    5   blue      seq 1
#define yellow 33
#define pink 32
#define orange 12
#define blue 14

#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define CAYENNE_LOG Serial
#include <CayenneMQTTESP32.h>

Adafruit_SGP30 sgp;
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// initialize the stepper library on pins 2 through 5:
Stepper myStepper(stepsPerRevolution,  yellow, blue, pink,  orange);
//Stepper myStepper(stepsPerRevolution,  yellow, pink, blue,  orange);
//Stepper myStepper(stepsPerRevolution,    pink, yellow, orange, blue);
//Stepper myStepper(stepsPerRevolution,    pink, yellow, blue, orange);
//Stepper myStepper(stepsPerRevolution,    pink, blue, orange, yellow );
//Stepper myStepper(stepsPerRevolution,    pink, blue, yellow, orange);
//Stepper myStepper(stepsPerRevolution,    pink, orange, blue, yellow );
//Stepper myStepper(stepsPerRevolution,    pink, orange,yellow, blue );




// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
// 
char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e"; 
char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
char clientID[] = "8f63aef0-0682-11e8-9094-712b97c1b05a";


/* change it with your ssid-password */
char ssid[] = "Casper";
////char wifiPassword[] = "B224F678E573DB5";
char wifiPassword[] = "abbaabbaff.";



unsigned long lastMillis = 0;




void setup()
{



 // set the speed at 60 rpm:
  myStepper.setSpeed(5);
  
  
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // setup I2C switch
  pinMode (EN_I2C1, OUTPUT); 
  pinMode (EN_I2C2, OUTPUT); 
  digitalWrite(EN_I2C1, HIGH);
  digitalWrite(EN_I2C2, HIGH);
  Wire.begin( SDA,SCL, 1000000); 
  Wire.setClock(100000); // choose 100 kHz I2C rate

 Serial.println("SGP30 test");


  digitalWrite(EN_I2C1, HIGH);
  digitalWrite(EN_I2C2, HIGH);
while (! sgp.begin()){
    Serial.println("Sensor1 not found :(");
    delay(1000);
   // while (1);
  }
  Serial.print("Found SGP30_1 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);


//  digitalWrite(EN_I2C1, HIGH);
//  digitalWrite(EN_I2C2, HIGH);
//while (! sgp.begin()){
//    Serial.println("Sensor2 not found :(");
//    delay(1000);
//   // while (1);
//  }
//  Serial.print("Found SGP30_2 serial #");
//  Serial.print(sgp.serialnumber[0], HEX);
//  Serial.print(sgp.serialnumber[1], HEX);
//  Serial.println(sgp.serialnumber[2], HEX);
  

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!



Cayenne.begin(username, password, clientID, ssid, wifiPassword);
Serial.print("Cayenne CFG done ");
myStepper.step(stepsPerRevolution);

}

int counter = 0;
int AV_CNT = 0;
int  TVOC1_AV =0;
int  eCO2_1_AV =0;
int RSSI_AV  = 0;

void loop()
{
    Cayenne.loop();
    
  if (current_pos != desired_pos) 
  {
  myStepper.step(current_pos - desired_pos);
  current_pos = desired_pos;
  }


  digitalWrite(EN_I2C1, HIGH);
  digitalWrite(EN_I2C2, HIGH);
  
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement1 failed");
    return;
  }
  Serial.print("TVOC_1 "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2_1 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

//  digitalWrite(EN_I2C1, HIGH);
//  digitalWrite(EN_I2C2, HIGH);
//
//  if (! sgp.IAQmeasure()) {
//    Serial.println("Measurement2 failed");
//    return;
//  }
//  Serial.print("TVOC_2 "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
//  Serial.print("eCO2_2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  
delay(1000);
//   counter++;
//  if (counter == 30) {
//    counter = 0;
//
//    uint16_t TVOC_base, eCO2_base;
//    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
//      Serial.println("Failed to get baseline readings");
//      return;
//    }
//    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
//    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
//}



  

 if (AV_CNT++<10)
  {
  TVOC1_AV += sgp.TVOC;
  eCO2_1_AV += sgp.eCO2;
  RSSI_AV += WiFi.RSSI();
  }
else
  {
    AV_CNT=0;
      Serial.print("PTVOC: ");  
      Serial.print(TVOC1_AV);
      Serial.println("  ppb"); 
            
      Serial.print("eCO2: ");  
      Serial.print(eCO2_1_AV);
      Serial.println("  ppm");   
      Serial.println();
     // Cayenne.virtualWrite(0, lastMillis);
      Cayenne.virtualWrite(3,(float)(TVOC1_AV/10));
      Cayenne.virtualWrite(4,(float)(eCO2_1_AV/10),"co2","ppm");
      Cayenne.virtualWrite(2, (float)RSSI_AV/10, "snr", "db");
      TVOC1_AV = 0;
      eCO2_1_AV = 0;
      RSSI_AV = 0;
  }
  


    }

CAYENNE_IN(5)
{
  // get value sent from dashboard
 desired_pos = getValue.asInt() * 50;
}

