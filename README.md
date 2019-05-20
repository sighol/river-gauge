# Gauge

Work in progress arduino code for a automatic water flow gauge.

Works with an Arduino nano.

# Installation

## Arch linux

Install the following packages:
- arduino
- arduino-avr-core

In the IDE (version 1.8.9 atm), pick `/dev/ttyUSB0` and use the bootloader: `ATmega328P (old bootloader)`


# Info

Arduino dokumentason for Nano: https://store.arduino.cc/arduino-nano

Nano klonen vi har: https://www.ebay.com/itm/MINI-USB-Nano-V3-0-ATmega328P-CH340G-5V-16M-Micro-controller-board-for-Arduino/381374550571?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2060353.m2749.l2649

Driver til brettet vårt:  https://sparks.gogo.co.nz/ch340.html

Sim 800L:
Pin 11=RX
Pin  3=TX
Manual: https://www.faranux.com/wp-content/uploads/2016/12/SIM800-Series_AT-Command-Manual_V1.09.pdf

Ultrasound sensor:
Pin 9=trigger
Pint 10=echo
Guide: https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/

