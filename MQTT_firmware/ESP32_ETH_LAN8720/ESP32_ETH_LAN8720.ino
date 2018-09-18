
#define CAYENNE_DEBUG



/*
    This sketch shows the Ethernet event usage

*/

#include <ETH.h>
//#include <WiFi.h>
//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#define CAYENNE_LOG Serial
#include <CayenneMQTTESP32.h>

/* create an instance of PubSubClient client */
WiFiClient espClient;


#define SERIAL2_RXPIN 16
#define SERIAL2_TXPIN 17

HardwareSerial Serial2(2);


#define LENG 9  //0xAA + 8 bytes s
unsigned char buf[LENG];
int PM2_5Value=0,PM2_5_AV_Value=0;         //define PM2.5 value of the air detector module
int PM10Value=0,PM10_AV_Value=0;         //define PM10 value of the air detector module
int AV_CNT=0;


// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
// EXTERNAL
//char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e"; 
//char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
//char clientID[] = "0a08e350-d2f3-11e7-9768-2143f8645011";
//INTERNAL
char username[] = "dbeef360-a5c5-11e7-bba6-6918eb39b85e";
char password[] = "4bf5c884c749da17023a9156a1b514beef743dbe";
char clientID[] = "7147f7f0-d2f2-11e7-ad90-91233d82c471";

/* change it with your ssid-password */
char ssid[] = "Casper";
//char wifiPassword[] = "B224F678E573DB5";
char wifiPassword[] = "abbaabbaff.";



static bool eth_connected = false;
long lastMsg = 0;
char msg[20];
/* topics */

unsigned long lastMillis = 0;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void setup()
{
   
    Serial2.begin(9600, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN);  //SERIAL1_RXPIN, SERIAL1_TXPIN
  Serial2.setTimeout(1500);    
  
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

      if (Serial2.available())  
  {
       if(Serial2.read()==0xAA)
      //  Serial.print("AA");
       {  
          Serial2.readBytes(buf,LENG);
    if(buf[0] == 0xC0){  
          if(checkValue(buf,LENG)){         
            PM2_5Value=transmitPM2_5(buf);//count PM2.5 value of the air detector module
            PM10Value=transmitPM10(buf); //count PM10 value of the air detector module 
          }      
      }     
    }
  }
 if (AV_CNT++<10)
  {
  PM2_5_AV_Value += PM2_5Value;
  PM10_AV_Value += PM10Value;
  }
else
  {
    AV_CNT=0;
      Serial.print("PM2.5 AV: ");  
      Serial.print(PM2_5_AV_Value);
      Serial.println("  ug/m3"); 
            
      Serial.print("PM10 AV: ");  
      Serial.print(PM10_AV_Value);
      Serial.println("  ug/m3");   
      Serial.println();
      Cayenne.virtualWrite(3,(float)(PM2_5_AV_Value/10));
      Cayenne.virtualWrite(4,(float)(PM10_AV_Value/10));
      PM2_5_AV_Value = 0;
      PM10_AV_Value = 0;
  }
  

  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);
     Serial.print(".");
    //Some examples of other functions you can use to send data.
    //Cayenne.celsiusWrite(1, 22.0);
    //Cayenne.luxWrite(2, 700);
    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  }

    
//  if (eth_connected) {
//    testClient("google.com", 80);
//  }
//  delay(10000);
}

char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;

//   for(int i=0; i<(LENG); i++){
//            Serial.print(buf[i], HEX);
//             }


  for(int i=1; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum&0xFF;
//   Serial.print("sum");
//  Serial.print(receiveSum, HEX);
//   Serial.print(thebuf[leng-1],HEX);
 
  if(receiveSum == thebuf[leng-2])  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
    
 //   Serial.print("sum OK");
  }
  return receiveflag;
}


//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val=((thebuf[2]<<8) + thebuf[1]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val=((thebuf[4]<<8) + thebuf[3]); //count PM10 value of the air detector module  
  return PM10Val;
}


