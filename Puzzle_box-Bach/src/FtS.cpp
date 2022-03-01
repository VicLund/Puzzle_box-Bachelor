#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_NeoPixel.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CDevice.h>

const int BlueButtonPin = 0;    // Blue ToggleSwitch pin
const int RedButtonPin = 1;     // Red ToggleSwitch pin
const int GreenButtonPin = 2;   // Green ToggleSwitch pin
const int OrangeButtonPin = 3;  // Orange ToggleSwitch pin
const int WhiteButtonPin = 4;   // White ToggleSwitch pin
int Allow_Start = 1;            // Determines if the game can be started (start button cant be used as a reset after the game has started)

int MyRegister[5] = {0};        // Saves the states of toggle switches in an array
int Progress[5] = {0};          // Saves the progress in an array
int SwitchPrevious[5] = {0};    // Savess the previous state of the toggle switches for the Switch-Case
const int UnlockingSequence[5] = {8, 7, 6, 5, 4};  // This array gives the numbers on the timer where individual switches can be toggled to get a "pass", UnlockingSequence[0] is for the blue toggle switch etc..

// variables will change:
int BlueButtonState = LOW;         // variable for reading the toggle switch status
int RedButtonState = LOW;          // variable for reading the toggle switch status
int GreenButtonState = LOW;        // variable for reading the toggle switch status
int OrangeButtonState = LOW;       // variable for reading the toggle switch status
int WhiteButtonState = LOW;        // variable for reading the toggle switch status

int BlueStateNow = 0;
int RedStateNow = 0;
int GreenStateNow = 0;
int OrangeStateNow = 0;
int WhiteStateNow = 0;

//boolean buttonState = LOW;
int BluePreviousState = LOW;    //Saves the previous state of the toggle switch 
int RedPreviousState = LOW;     //Saves the previous state of the toggle switch
int GreenPreviousState = LOW;   //Saves the previous state of the toggle switch
int OrangePreviousState = LOW;  //Saves the previous state of the toggle switch
int WhitePreviousState = LOW;   //Saves the previous state of the toggle switch 


unsigned long DebounceDelay = 50;
unsigned long DebounceTimer = 0;
unsigned long LoopDelay = 1000;             // ticks down at 1000ms rate (normal speed)
unsigned long LoopTimer = 0;
unsigned long PassDebounceTimer = 0;        
unsigned long PassDebounceDelay = 50;
Adafruit_7segment matrix = Adafruit_7segment();
boolean drawDots = true;

//Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, NEOPIN, NEO_RGB + NEO_KHZ800);
//Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void GameProgress()
{
  Serial.println("Pass ");
  delay(50);
}

void AccelerateTimer()
{
  Serial.println("Fail ");
  if (LoopDelay > 200)
  {
    MyRegister[0] = 0; 
    LoopDelay = (LoopDelay - 20); // Timer ticks down at a faster rate after failure (wrong input)
    Serial.print("LoopDelay: ");
    Serial.println(LoopDelay, DEC);
  }
  delay(50);
}

void DebounceBlue()
{
  BlueStateNow = digitalRead(BlueButtonPin);
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
          MyRegister[0] = 0;   // OFF
        }
      if (BlueButtonState == HIGH) 
        {
          MyRegister[0] = 1;   // ON
        }
    }
  }
  BluePreviousState = BlueStateNow; 
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
          MyRegister[1] = 0;   // OFF
        }
      if (RedButtonState == HIGH) 
        {
          MyRegister[1] = 1;   // ON
        }
    }
  }
  RedPreviousState = RedStateNow; 
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
          MyRegister[2] = 0;   // OFF
        }
      if (GreenButtonState == HIGH) 
        {
          MyRegister[2] = 1;   // ON
        }
    }
  }
  GreenPreviousState = GreenStateNow; 
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
          MyRegister[3] = 0;  //OFF
        }
      if (OrangeButtonState == HIGH) 
        {
          MyRegister[3] = 1;  //ON
        }
    }
  }
  OrangePreviousState = OrangeStateNow; 
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
          MyRegister[4] = 0;   //Switchen er NO, og når den er switchen ON så kobles den til jord, så vi får ingen lys i LED. Derfor er det HIGH state (siden LED da lyser) som skal registreres i koden, og brukes til å registrere at den er "PÅ", mens signalet egentlig er lavt
        }
      if (WhiteButtonState == HIGH) 
        {
          MyRegister[4] = 1;  // ON
        }
    }
  }
  WhitePreviousState = WhiteStateNow; 
}



void setup() {

matrix.begin(0x70);
Serial.begin(9600);
pinMode(BlueButtonPin, INPUT_PULLUP);
pinMode(RedButtonPin, INPUT_PULLUP);
pinMode(GreenButtonPin, INPUT_PULLUP);
pinMode(OrangeButtonPin, INPUT_PULLUP);
pinMode(WhiteButtonPin, INPUT_PULLUP);
}

void loop() 
{
  for (uint16_t counterA = 3, counterB = 0, counterC = 0, counterD = 0; (counterA + counterB + counterC + counterD) >= 0; counterD--) 
  {

    if (Allow_Start == 0)
    {
      Serial.println("You ran out of time, restart the game to try again");
      matrix.print("DONE");
      matrix.writeDisplay();
      delay(200000);
    }

  if ((Progress[0] == 1 && Progress[1] == 1 && Progress[2] == 1 && Progress[3] == 1 && Progress[4] == 1) && (MyRegister[0] == 1 && MyRegister[1] == 1 && MyRegister[2] == 1 && MyRegister[3] == 1 && MyRegister[4] == 1))
     {
       Serial.println("Congratulations, you beat the game!");
       matrix.print("DONE");
      }

      matrix.writeDigitNum(0, (counterA));
      matrix.writeDigitNum(1, (counterB));
      matrix.drawColon(drawDots);
      matrix.writeDigitNum(3, (counterC));
      matrix.writeDigitNum(4, (counterD));
      matrix.writeDisplay();
      LoopTimer = millis();
  
    while ((millis() - LoopTimer) < LoopDelay)
    { 
        if (counterC > 6)
          {
           counterC = 6;
          }
       if (counterD == 0 && counterC != 0)
          { 
           counterD = 10;
           counterC--;
          }  
       if (counterC == 0 && counterD == 0 && counterB != 0)
         {
          counterC = 5;
          counterD = 10;
          counterB--;
         }
       if (counterB == 0 && counterC == 0 && counterD == 0 && counterA != 0)
         {
          counterD = 10;
          counterC = 5;
          counterB = 9;
          counterA--;
         }
       if(counterB == 0 && counterC == 0 && counterD == 0 && counterA == 0)
         {
           Serial.println("GAME OVER");
           Allow_Start = 0;
           break;
         }

          DebounceBlue();
          DebounceRed();
          DebounceGreen();
          DebounceOrange();
          DebounceWhite();

      for (int i = 0, c = 0; i < 5; i++, c++)
      {
        switch(MyRegister[i])
        {
          case 0:
          SwitchPrevious[c] = 0;
          break;
          case 1:
          if ((counterA != UnlockingSequence[i] && counterB != UnlockingSequence[i] && counterC != UnlockingSequence[i] && counterD != UnlockingSequence[i]) && (SwitchPrevious[c] != MyRegister[i]))     // IF statement for when the switch is flipped at the incorrect time
           {
              AccelerateTimer();
              SwitchPrevious[c] = 1;
              memset(Progress, 0, 5);
              break;
           }
          if ((counterA == UnlockingSequence[i] || counterB == UnlockingSequence[i] || counterC == UnlockingSequence[i] || counterD == UnlockingSequence[i]) && (SwitchPrevious[c] != MyRegister[i]) && MyRegister[i] == 1)
           {
             GameProgress();
             SwitchPrevious[c] = 1;
             Progress[i] = 1;
           break;
           }
          else {break;}

          default:
           break;
        }  
      }
    }
      


       


  }


}