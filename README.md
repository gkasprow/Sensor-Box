# Sensor-Box
Multichannel standarized enviorement for home automation and other MQTT applications

This is design of modular, DIN-based system for hme automation and industrial control.
There are several great open-source SW enviorements like Cayenne, Blynk, Node Red. They are used to control breadboard stuff built with Arduino, RPI and extension modules.
What I miss the most is well designed hardware that fully explores potential of the software.
So I designed several base and extension modules. The idea is very simple - there is controller module based on Arduino, ESP32 or Linux platform.
From its left side one plugs connectivity modules like Ethernet, LoRa, GPRS and supply like DC, AC, PoE, battery backup.
From its right side one can plug any number of extensions like energy meters, relays, Lighting drivers. One can also connect external sensors using CAN bus.
Several existing technologies were used to build the system, to count a few: HAPCAN, Arduino, Node Red, Cayenne.
More information is available on Wiki page.
