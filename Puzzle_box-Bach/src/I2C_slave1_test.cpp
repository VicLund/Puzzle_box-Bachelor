#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>

//LED strip defines
#define PIN 6
#define NUM_LEDS 92

//Define variable to receive
byte receiveFromMaster = 0;

//Define variable to send
byte sendToMaster = 0;

//Checking to make sure that the function is actually done
boolean lightsFinished = false;

//Forward declare functions
void receiveEvent(int);
void requestEvent();
void redLight();
void blueLight();

Adafruit_NeoPixel pixels(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);

  //I2C setup
  Wire.begin(0x01);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //NeoPixel setup
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void loop() {

}

void receiveEvent(int) {
  receiveFromMaster = Wire.read();
  if (receiveFromMaster == 1) {
    Serial.println("Red light turning on");
    redLight();
  }
  else if (receiveFromMaster == 2) {
    Serial.println("Blue light turning on");
    blueLight();
  }
}

void requestEvent() {
  while(lightsFinished) {
    if (sendToMaster == 0) {
      Serial.println("Sent 'No Signal' to Master");
      Wire.write(sendToMaster);
    }
    else if (sendToMaster == 1) {
      Serial.println("Sent 'Red finished' to Master");
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    else if (sendToMaster == 2) {
      Serial.println("Sent 'Blue finished' to Master");
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    lightsFinished = false;
  }
}

void redLight() {
  sendToMaster = 1;
  pixels.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(1000);
  lightsFinished = true;
}

void blueLight() {
  sendToMaster = 2;
  pixels.clear();
  for (int j = 0; j < NUM_LEDS; j++) {
    pixels.setPixelColor(j, pixels.Color(0, 0, 255));
    pixels.show();
  }
  delay(1000);
  for (int j = 0; j < NUM_LEDS; j++) {
    pixels.setPixelColor(j, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(1000);
  for (int j = 0; j < NUM_LEDS; j++) {
    pixels.setPixelColor(j, pixels.Color(0, 0, 255));
    pixels.show();
  }
  delay(1000);
  for (int j = 0; j < NUM_LEDS; j++) {
    pixels.setPixelColor(j, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(1000);
  lightsFinished = true;
}