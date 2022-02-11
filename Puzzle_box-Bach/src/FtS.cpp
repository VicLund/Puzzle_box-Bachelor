#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
//#include <DFRobot_I2CMultiplexer.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>

const int BlueButtonPin = 0;    // Blue ToggleSwitch pin
const int RedButtonPin = 1;     // Red ToggleSwitch pin
const int GreenButtonPin = 2;   // Green ToggleSwitch pin
const int OrangeButtonPin = 3;  // Orange ToggleSwitch pin
const int WhiteButtonPin = 4;   // White ToggleSwitch pin
    

// variables will change:
int BlueButtonState = LOW;         // variable for reading the toggle switch status
int RedButtonState = LOW;          // variable for reading the toggle switch status
int GreenButtonState = LOW;        // variable for reading the toggle switch status
int OrangeButtonState = LOW;       // variable for reading the toggle switch status
int WhiteButtonState = LOW;        // variable for reading the toggle switch status

//boolean buttonState = LOW;
boolean BluePreviousState = LOW;    //Saves the previous state of the toggle switch
boolean RedPreviousState = LOW;     //Saves the previous state of the toggle switch
boolean GreenPreviousState = LOW;   //Saves the previous state of the toggle switch
boolean OrangePreviousState = LOW;  //Saves the previous state of the toggle switch
boolean WhitePreviousState = LOW;   //Saves the previous state of the toggle switch 






boolean debounceBlue(boolean state)
{
  boolean stateNow = digitalRead(BlueButtonPin);
  if(state!=stateNow)
  {
    delay(50);
    stateNow = digitalRead(BlueButtonPin);
  }
  return stateNow;
  
}

boolean debounceRed(boolean state)
{
  boolean stateNow = digitalRead(RedButtonPin);
  if(state!=stateNow)
  {
    delay(50);
    stateNow = digitalRead(RedButtonPin);
  }
  return stateNow;
  
}

boolean debounceGreen(boolean state)
{
  boolean stateNow = digitalRead(GreenButtonPin);
  if(state!=stateNow)
  {
    delay(50);
    stateNow = digitalRead(GreenButtonPin);
  }
  return stateNow;
  
}

boolean debounceOrange(boolean state)
{
  boolean stateNow = digitalRead(OrangeButtonPin);
  if(state!=stateNow)
  {
    delay(50);
    stateNow = digitalRead(OrangeButtonPin);
  }
  return stateNow;
  
}

boolean debounceWhite(boolean state)
{
  boolean stateNow = digitalRead(WhiteButtonPin);
  if(state!=stateNow)
  {
    delay(50);
    stateNow = digitalRead(WhiteButtonPin);
  }
  return stateNow;
  
}







void setup() {
  pinMode(BlueButtonPin, INPUT);
  pinMode(RedButtonPin, INPUT);
  pinMode(GreenButtonPin, INPUT);
  pinMode(OrangeButtonPin, INPUT);
  pinMode(WhiteButtonPin, INPUT);
  Serial.begin(9600);
}

void loop() {


  BlueButtonState = digitalRead(BlueButtonPin);
  RedButtonState = digitalRead(BlueButtonPin);
  GreenButtonState = digitalRead(BlueButtonPin);
  OrangeButtonState = digitalRead(BlueButtonPin);
  WhiteButtonState = digitalRead(BlueButtonPin);

  //--------------------------------------------------------------------//
  
  if(debounceBlue(BlueButtonState) == HIGH && BluePreviousState == LOW)
  {
    Serial.println("Blue HIGH");
    BluePreviousState = BlueButtonState;
  }
  //if(debounceButton(BlueButtonState) == LOW && BluePreviousState == HIGH)
  else {Serial.println("Blue LOW");BluePreviousState = BlueButtonState;}

    //--------------------------------------------------------------------//
  
  if(debounceRed(RedButtonState) == HIGH && RedPreviousState == LOW)
  {
    Serial.println("Red HIGH");
    RedPreviousState = RedButtonState;
  }
  //if(debounceButton(BlueButtonState) == LOW && BluePreviousState == HIGH)
  else 
    {
    Serial.println("Red LOW");
    RedPreviousState = RedButtonState;
    }

    //--------------------------------------------------------------------//
  
  if(debounceGreen(GreenButtonState) == HIGH && GreenPreviousState == LOW)
  {
    Serial.println("Green HIGH");
    GreenPreviousState = GreenButtonState;
  }
  //if(debounceButton(BlueButtonState) == LOW && BluePreviousState == HIGH)
  else 
    {
    Serial.println("Green LOW");
    GreenPreviousState = GreenButtonState;
    }

     //--------------------------------------------------------------------//
  
  if(debounceOrange(OrangeButtonState) == HIGH && OrangePreviousState == LOW)
  {
    Serial.println("Orange HIGH");
    OrangePreviousState = OrangeButtonState;
  }
  //if(debounceButton(BlueButtonState) == LOW && BluePreviousState == HIGH)
  else 
    {
    Serial.println("Orange LOW");
    OrangePreviousState = OrangeButtonState;
    }

      //--------------------------------------------------------------------//
  
  if(debounceWhite(WhiteButtonState) == HIGH && WhitePreviousState == LOW)
  {
    Serial.println("White HIGH");
    WhitePreviousState = WhiteButtonState;
  }
  //if(debounceButton(BlueButtonState) == LOW && BluePreviousState == HIGH)
  else 
    {
    Serial.println("White LOW");
    WhitePreviousState = WhiteButtonState;
    }

  delay(1000);
}

