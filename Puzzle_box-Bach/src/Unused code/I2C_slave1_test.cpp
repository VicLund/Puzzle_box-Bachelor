#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>

//Define variable to receive
byte receiveFromMaster = 0;

//Define variable to send
byte sendToMaster = 0;

//Checking to make sure that the function is actually done
boolean lightsFinished = false;

//Forward declare functions
void receiveEvent(int);
void requestEvent();
void builtinLED_RtN();

void setup() {
  Serial.begin(9600);

  //I2C setup
  Wire.begin(0x01);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  /*int time = millis()/1000;
  
  //print the time from last reset
  Serial.print("I was reset ");
  Serial.print(time);
  Serial.println(" seconds ago.");
  Serial.println("Will you stop Dave. I am afraid.");
  delay(1000);*/
}

void receiveEvent(int) {
  receiveFromMaster = Wire.read();
  if (receiveFromMaster == 1) {
    Serial.println("RtN running - Builtin LED turning on");
    builtinLED_RtN();
  }
}

void requestEvent() {
  while(lightsFinished) {
    if (sendToMaster == 0) {
      Serial.println("Sent 'No Signal' to Master");
      Wire.write(sendToMaster);
    }
    else if (sendToMaster == 1) {
      Serial.println("Sent 'Finished RtN' to Master");
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    lightsFinished = false;
  }
}

void builtinLED_RtN() {
  sendToMaster = 1;
  int n = 30;
  for(int i = 0; i < n; i++){
    Serial.print("RtN Lights have blinked: ");
    Serial.print(i+1);
    Serial.print(" times. Total blinks is ");
    Serial.println(n);
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(2000);
  }
  delay(1000);
  lightsFinished = true;
}