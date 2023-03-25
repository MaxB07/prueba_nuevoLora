#include <Arduino.h>           
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <LoRa.h>
#include <SPI.h>  
#include<Wire.h>
#include<ADXL345_WE.h>
#include <util/delay.h>

#define SF 7
#define CR 5
#define BW 500E3
#define pin_interrupt 2  
#define LORA_FRECUENCY 433E6
#define ADXL345_I2CADDR 0x53 // 0x1D if SDO = HIGH
#define DIO0 0

String outgoing;              // outgoing message
String message = "";
String datos_almacenados = "";

uint8_t packetSize, counter = 0;
uint32_t msgCount = 0;            // count of outgoing messages
byte localAddress = 0xA2;     // address of this device
byte destination = 0xC0;      // destination to send to
float g_value = 0.0;
const int muestras = 40; 