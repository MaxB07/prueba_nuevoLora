//Atmega328P del arduino nano -----> boton blanco

// ********************************** Nodo Aceleración LoRa 2 ************************************************


#include <communication.h>

ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR);

void sendMessage(String outgoing);
void tramaAcel();
void onReceive(int packetSize);
void lowPower();
void sendMessage(String outgoing);


void setup() {

  Wire.begin();
  SPI.begin();
  lowPower();

  if (!LoRa.begin(LORA_FRECUENCY))  return;     // initialize ratio at 433 MHz          if failed, do nothing

  if (!myAcc.init()) return;

  LoRa.setSpreadingFactor(SF);
  LoRa.setSignalBandwidth(BW);
  LoRa.setCodingRate4(CR);
  myAcc.setDataRate(ADXL345_DATA_RATE_200);
  myAcc.setRange(ADXL345_RANGE_16G);
  LoRa.receive();

}

void loop() {
  onReceive(LoRa.parsePacket());
}


void sendMessage(String outgoing) {

  if(counter <= muestras){
    datos_almacenados += outgoing;
  }
  else{
    LoRa.beginPacket();                   // start packet
    LoRa.write(destination);              // add destination address
    LoRa.write(localAddress);             // add sender address
    LoRa.write(msgCount);                 // add message ID
    LoRa.write(datos_almacenados.length());        // add payload length
    LoRa.print(datos_almacenados);                 // add payload
    LoRa.endPacket();                     // finish packet and send it
    msgCount++; 
    counter = 0;
    datos_almacenados = "";
    LoRa.sleep();
    LoRa.end();
    MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    myAcc.setMeasureMode(false);
  }
  counter++;
}

void onReceive(int packetSize) {
  if (packetSize == 0){
 //   power_all_disable();
    return;
  } 

  byte recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  uint8_t incomingMsgId = LoRa.read();     // incoming msg ID
  uint8_t incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress) {
//    LoRa.sleep();
    return;                             // skip rest of function
  }
  
  if (incomingLength != incoming.length()) {   // check length for error
    LoRa.idle();
    return;                             // skip rest of function
  }
 // power_all_enable();
  tramaAcel();

  LoRa.receive();
}

void tramaAcel(){
  for(uint8_t i=0;i<=muestras;i++){
    xyzFloat g = myAcc.getGValues();
    myAcc.setMeasureMode(true);
    g_value = 0;
    if(i <= muestras/2){
      g_value = g.x;
    }
    else{
      g_value = g.z;
    }
    message = g_value;
    message += " "; 
    sendMessage(message);
    _delay_ms(10);
  }
}

void lowPower(){
 
  cli();
  CLKPR = (1<<CLKPS1); // el preescaler del reloj principal esta en 64
  CLKPR |= (1<<CLKPCE);
  sei();
 
  cli();
  ADCSRA &= ~_BV(ADEN);
//  DIDR0 = 0x0F; 
  ADCSRB = _BV(ACME);  //deshabilota el comparador interno
  ACSR = _BV(ACD);
  PRR = _BV(PRADC);
  MCUCR &= ~(1<<BODSE);
  MCUCR |= 1<<BODS;
  MCUSR &= ~(1<<WDRF);
  WDTCSR = 0x00;
  sei();
  

}


/*
#include <Arduino.h>
#include <Wire.h>
byte adxl345_adress = 0x53;
void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);

  Wire.beginTransmission(adxl345_adress);
//  Wire.write(0xA6);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(adxl345_adress, 1);
  byte id = Wire.read();

// Configurar BW_Rate en 200 Hz
  Wire.beginTransmission(adxl345_adress);
  Wire.write(0x2c);
  Wire.write(B00001100);
  Wire.endTransmission();
  delay(10);

}

void loop(){
  Wire.beginTransmission(adxl345_adress);
  Wire.write(0x2c); 
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(adxl345_adress, 1);
  int dr = Wire.read();
  Serial.println(dr,HEX);
}
*/