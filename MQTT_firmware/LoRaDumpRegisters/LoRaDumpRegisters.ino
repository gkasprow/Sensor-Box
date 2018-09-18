/*
  LoRa register dump

  This examples shows how to inspect and output the LoRa radio's
  registers on the Serial interface
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

void setup() {
  SerialUSB.begin(9600);               // initialize serial
  pinMode(13, OUTPUT); 
  digitalWrite(13, 1);
  while (!SerialUSB);

  


  SerialUSB.println("LoRa Dump Registers");

  // override the default CS, reset, and IRQ pins (optional)
  // LoRa.setPins(7, 6, 1); // set CS, reset, IRQ pin

  if (!LoRa.begin(915E6)) {         // initialize ratio at 915 MHz
    SerialUSB.println("LoRa init failed. Check your connections.");
    while (true);                   // if failed, do nothing
  }

  LoRa.dumpRegisters(SerialUSB);
}


void loop() {
}

