#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>

//I2C slaves defines
#define RtN_Slave 0x01
#define FtS_Slave 0x02

//Define a variable to send
byte sendRed = 1;
byte sendBlue = 2;
byte sendBuiltinLED = 3;

//Define a variable to receive
byte masterReceive1;
byte masterReceive2;

//Forward declaring the functions used
void debounceSendRedButton();
void debounceSendBlueButton();
void debounceSendBuiltinLEDButtonButton();
void requestSlave1();
void requestSlave2();
void noSignal_1();
void noSignal_2();

//Button variables
const int sendRedButton = A1;
int sendRedButton_state;
int sendRedButton_lastState = LOW;

const int sendBlueButton = A2;
int sendBlueButton_state;
int sendBlueButton_lastState = LOW;

const int sendBuiltinLEDButton = A3;
int sendBuiltinLEDButton_state;
int sendBuiltinLEDButton_lastState = LOW;

//Debounce
unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;



void setup() {
  Serial.begin(9600);

  Wire.begin();

  pinMode(sendRedButton, INPUT_PULLUP);
  pinMode(sendBlueButton, INPUT_PULLUP);
  pinMode(sendBuiltinLEDButton, INPUT_PULLUP);

}

void loop() {
  debounceSendRedButton();
  debounceSendBlueButton();
  debounceSendBuiltinLEDButtonButton();
}

void debounceSendRedButton(){
  int sendRedButton_reading = digitalRead(sendRedButton);

  if(sendRedButton_reading != sendRedButton_lastState){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){
    if(sendRedButton_reading != sendRedButton_state){
      sendRedButton_state = sendRedButton_reading;
      if(sendRedButton_state == LOW){
        Serial.println("Signal red sent to slave");
        //slaveID = 1;
        //Send signal to turn on LED strip
        delay(100);
        Wire.beginTransmission(RtN_Slave);
        Wire.write(sendRed);
        Wire.endTransmission();
        requestSlave1();
      }
    }
  }
  sendRedButton_lastState = sendRedButton_reading;
}

void debounceSendBlueButton(){
  int sendBlueButton_reading = digitalRead(sendBlueButton);

  if(sendBlueButton_reading != sendBlueButton_lastState){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){
    if(sendBlueButton_reading != sendBlueButton_state){
      sendBlueButton_state = sendBlueButton_reading;
      if(sendBlueButton_state == LOW){
        Serial.println("Signal blue sent to slave");
        //slaveID = 1;
        //Send signal to turn on LED strip
        delay(100);
        Wire.beginTransmission(RtN_Slave);
        Wire.write(sendBlue);
        Wire.endTransmission();
        requestSlave1();
      }
    }
  }
  sendBlueButton_lastState = sendBlueButton_reading;
}

void debounceSendBuiltinLEDButtonButton(){
  int sendBuiltinLEDButton_reading = digitalRead(sendBuiltinLEDButton);

  if(sendBuiltinLEDButton_reading != sendBuiltinLEDButton_lastState){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){
    if(sendBuiltinLEDButton_reading != sendBuiltinLEDButton_state){
      sendBuiltinLEDButton_state = sendBuiltinLEDButton_reading;
      if(sendBuiltinLEDButton_state == LOW){
        Serial.println("Signal builtin LED sent to slave");
        //slaveID = 2;
        //Send signal to turn on LED strip
        delay(100);
        Wire.beginTransmission(FtS_Slave);
        Wire.write(sendBuiltinLED);
        Wire.endTransmission();
        requestSlave2();
      }
    }
  }
  sendBuiltinLEDButton_lastState = sendBuiltinLEDButton_reading;
}

void requestSlave1(){
  //Request and wait for signal from slave
  Wire.requestFrom(RtN_Slave, 1);
  masterReceive1 = Wire.read();
  if(masterReceive1 == 0){
    noSignal_1(); 
  }
  else if(masterReceive1 == 1){
    Serial.println("Red lights finished on slave 1");
  }
  else if(masterReceive1 == 2){
    Serial.println("Blue lights finished on slave 1");
  }
}

void requestSlave2(){
  Wire.requestFrom(FtS_Slave, 1);
  masterReceive2 = Wire.read();
  if(masterReceive2 == 0){
    noSignal_2();
  }
  else if(masterReceive2 == 3){
    Serial.println("Builtin LED finished on slave 2");
  }
}

void noSignal_1(){
  requestSlave1();
}

void noSignal_2(){
  requestSlave2();
}