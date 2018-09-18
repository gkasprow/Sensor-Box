



#define CAYENNE_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneMQTTESP8266.h>
#include <Ticker.h>
#define TEMP_PIN V2
const int tmpPin = 12;
#define LED D0
#define timer_per 500000  //50 ms   0.2us/tick
OneWire oneWire(tmpPin);
DallasTemperature sensors(&oneWire);


// WiFi network info.
char ssid[] = "Casper2";
char wifiPassword[] = "abbaabbaff.";


// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e";
char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
char clientID[] = "7069bfd0-9106-11e8-87b0-97a7dd91c15a";

unsigned long lastMillis = 0;
// Virtual Pin of the DS18B20 widget.
#define TEMP_PIN V2
#define VIRTUAL_PIN V1
int volume;
int flow;
int valve_desired_pos;
int valve_current_pos;


bool tickOccured;
bool dmpulse;
int cnt_60s = 0;
int cnt_1s = 0;
int volume_old;

// interrupt routines

//=======================================================================

// ISR every 50ms
void ICACHE_RAM_ATTR onTimerISR(){
    //digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    tickOccured = true;
    timer1_write(timer_per);

    if (dmpulse == true) {
      dmpulse = false;
      volume ++;
    }
    
    if (cnt_60s == 600) {
      //digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
      cnt_60s = 0;
      flow = volume - volume_old;
      volume_old = volume;
    }
    cnt_60s++;

   // if (cnt_1s == 10) {
   //   digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    //  cnt_1s = 0;

     if (valve_current_pos != valve_desired_pos) 
    {
    if (valve_current_pos >= valve_desired_pos){
      digitalWrite(15, HIGH);
      digitalWrite(13, LOW);
      valve_current_pos--;
    }
       if (valve_current_pos < valve_desired_pos){
      digitalWrite(15, LOW);
      digitalWrite(13, HIGH);
      valve_current_pos++;
    }
    }
    else {
      digitalWrite(15, LOW);
      digitalWrite(13, LOW);
    }


    //}
   // cnt_1s++;




    
    
}


void dm_isr() {
  dmpulse = true;

}

void right() {
  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
}



void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW); // GND for flow meter contact to mitigate startup pullup requirement for 0 and 2
  pinMode(2, INPUT);
  pinMode(LED,OUTPUT);
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial.println("started");
  attachInterrupt(2, dm_isr, FALLING);
  //attachInterrupt(0, right, FALLING);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  //toggle();
  tickOccured = false;
  volume_old=0;
  volume = 0;
  
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(timer_per); 

    valve_desired_pos=0;
    valve_current_pos=200; // force closing at startup

}

// the loop function runs over and over again forever
void loop() {
  Cayenne.loop();
//  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
//  digitalWrite(15, LOW);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
//  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second

  //if (digitalRead(0) == LOW)  {Serial.println("pin0");}
  //if (digitalRea

    //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
          lastMillis = millis();
          sensors.requestTemperatures();
          Serial.println("tmp1");
          Serial.println( sensors.getTempCByIndex(0));
          Serial.println("tmp2");
          Serial.println( sensors.getTempCByIndex(1));
          Cayenne.celsiusWrite(1, sensors.getTempCByIndex(0));
          Cayenne.celsiusWrite(2, sensors.getTempCByIndex(1));
          Cayenne.virtualWrite(3, valve_current_pos);
          Cayenne.virtualWrite(4, WiFi.RSSI(), "snr", "db");
          Cayenne.virtualWrite(7, volume, "Liter", "l");
          Cayenne.virtualWrite(8, flow, "Liter", "l");

          }

 if (tickOccured == true)
 {

    Serial.println("volume");
    Serial.println(volume);
    Serial.println("flow");
    Serial.println(flow);
    tickOccured = false;
    Serial.println("pos");
    Serial.println(valve_current_pos);

 }


  

}


CAYENNE_IN(5)
{
  // get value sent from dashboard
    
    Serial.println("in5");
Serial.println( getValue.asInt());
 valve_desired_pos = getValue.asInt() ;
 if (valve_desired_pos >100){
  valve_desired_pos = 100;
 }
 
}


CAYENNE_IN(6)
{
  // get value sent from dashboard
  int currentValue = getValue.asInt(); // 0 to 1
  if (currentValue == 1) {
    valve_desired_pos=0;
    valve_current_pos=200; // force closing at startup
  }
}



