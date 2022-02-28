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
#include <Adafruit_GFX.h>
// #include <Adafruit_BusIO_Register.h>
// #include <Adafruit_I2CRegister.h>
// #include <Adafruit_SPIDevice.h>
// #include <Adafruit_I2CDevice.h>

const int BlueButtonPin = 0;    // Blue ToggleSwitch pin
const int RedButtonPin = 1;     // Red ToggleSwitch pin
const int GreenButtonPin = 2;   // Green ToggleSwitch pin
const int OrangeButtonPin = 3;  // Orange ToggleSwitch pin
const int WhiteButtonPin = 4;   // White ToggleSwitch pin

const int ButtonPins[5] = {0, 1, 2, 3, 4};
int MyRegister[5] = {0};        // Saves the states of toggle switches in an array

int Allow_Start = 1;            // Determines if the game can be started (start button cant be used as a reset after the game has started)
int Fail = 0;                   // If fail != 0, it means the switch was toggled at the wrong moment (wrong number on timer)
int Success = 0;                // Counts successful toggles 
int Count = 0;                  // Counts the number of switches toggled / flipped

// variables will change:
int BlueButtonState = LOW;         // variable for reading the toggle switch status
int RedButtonState = LOW;          // variable for reading the toggle switch status
int GreenButtonState = LOW;        // variable for reading the toggle switch status
int OrangeButtonState = LOW;       // variable for reading the toggle switch status
int WhiteButtonState = LOW;        // variable for reading the toggle switch status

//boolean buttonState = LOW;
int BluePreviousState = LOW;    //Saves the previous state of the toggle switch
int RedPreviousState = LOW;     //Saves the previous state of the toggle switch
int GreenPreviousState = LOW;   //Saves the previous state of the toggle switch
int OrangePreviousState = LOW;  //Saves the previous state of the toggle switch
int WhitePreviousState = LOW;   //Saves the previous state of the toggle switch 

unsigned long DebounceDelay = 50;
unsigned long DebounceTimer = 0;
unsigned int CountdownTimer;  //how much time the player has to solve the puzzle
const unsigned long CountdownSpeed1 = 1000; // ticks down at 1000ms rate (normal speed)
const unsigned long CountdownSpeed2 = 10;   // Timer ticks down at a faster rate after failure (wrong input)
unsigned int TimerCountdownSpeed = CountdownSpeed1; //How mu

//Switchen er NO, så, når den fysisk er switched ON så kobles pinnen til jord, og vi får ingen lys i LED. Derfor er det OFF state (siden LED da lyser) som skal registreres i koden, og brukes til å registrere at den er "PÅ", mens signalet egentlig er lavt

void DebounceBlue()
{
  int BlueStateNow = digitalRead(BlueButtonPin);
  if(BlueStateNow != BluePreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(BlueStateNow != BlueButtonState)
    {
      BlueButtonState = BlueStateNow;
      if (BlueButtonState == LOW)
        {
          Serial.println("Blue switch OFF");
        }
      if (BlueButtonState == HIGH)
        {
          Serial.println("Blue switch ON");
        }
    }
  }
  BluePreviousState = BlueStateNow; 
  MyRegister[0] = BlueStateNow; 
}

void DebounceRed()
{
  int RedStateNow = digitalRead(RedButtonPin);
  if(RedStateNow != RedPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(RedStateNow != RedButtonState)
    {
      RedButtonState = RedStateNow;
      if (RedButtonState == LOW)
        {
          Serial.println("Red switch OFF");
        }
      if (RedButtonState == HIGH) 
        {
          Serial.println("Red switch ON");
        }
    }
  }
  RedPreviousState = RedStateNow;
  MyRegister[1] = RedStateNow; 
}

void DebounceGreen()
{
  int GreenStateNow = digitalRead(GreenButtonPin);
  if(GreenStateNow != GreenPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(GreenStateNow != GreenButtonState)
    {
      GreenButtonState = GreenStateNow;
      if (GreenButtonState == LOW)
        {
          Serial.println("Green switch OFF");
        }
      if (GreenButtonState == HIGH) 
        {
          Serial.println("Green switch ON");
        }
    }
  }
  GreenPreviousState = GreenStateNow;
  MyRegister[2] = GreenStateNow; 
}

void DebounceOrange()
{
  int OrangeStateNow = digitalRead(OrangeButtonPin);
  if(OrangeStateNow != OrangePreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(OrangeStateNow != OrangeButtonState)
    {
      OrangeButtonState = OrangeStateNow;
      if (OrangeButtonState == LOW)
        {
          Serial.println("Orange switch OFF");
        }
      if (OrangeButtonState == HIGH) 
        {
          Serial.println("Orange switch ON");
        }
    }
  }
  OrangePreviousState = OrangeStateNow; 
  MyRegister[3] = OrangeStateNow;
}

void DebounceWhite()
{
  int WhiteStateNow = digitalRead(WhiteButtonPin);
  if(WhiteStateNow != WhitePreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(WhiteStateNow != WhiteButtonState)
    {
      WhiteButtonState = WhiteStateNow;
      if (WhiteButtonState == LOW)
        {
          Serial.println("White switch OFF");   //Switchen er NO, og når den er switchen ON så kobles den til jord, så vi får ingen lys i LED. Derfor er det HIGH state (siden LED da lyser) som skal registreres i koden, og brukes til å registrere at den er "PÅ", mens signalet egentlig er lavt
        }
      if (WhiteButtonState == HIGH) 
        {
          Serial.println("White switch ON");
        }
    }
  }
  WhitePreviousState = WhiteStateNow;
  MyRegister[4] = WhiteStateNow; 
}


void setup() {
  Serial.begin(9600);
  pinMode(BlueButtonPin, INPUT_PULLUP);
  pinMode(RedButtonPin, INPUT_PULLUP);
  pinMode(GreenButtonPin, INPUT_PULLUP);
  pinMode(OrangeButtonPin, INPUT_PULLUP);
  pinMode(WhiteButtonPin, INPUT_PULLUP);
}

void loop() 
{
DebounceBlue();
DebounceRed();
DebounceGreen();
DebounceOrange();
DebounceWhite();

}






