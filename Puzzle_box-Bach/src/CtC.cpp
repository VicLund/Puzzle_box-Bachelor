#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_NeoPixel.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CDevice.h>
#include <TM1637Display.h>


// Module connection pins (Digital Pins)
#define CLK A5
#define DIO A6
#define BRIGHTNESS 50 
 // Data in pins
const int           RotaryNC_Pin            = 0;                                                 // Data in Pin r
const int           RotarySW1_Pin           = 1;                                                 // Data in Pin 
const int           RotarySW2_Pin           = 2;                                                 // Data in Pin 
int                 RotarySelect            = HIGH;                                              //
int                 RotaryRight             = HIGH;                                              //
int                 RotaryLeft              = HIGH;                                              //
int                 SelectDigit             = 0;                                                 //
int                 Attempts                = 6;                                                 //
int                 SelectStateNow          = 0;                                                 //
int                 RightStateNow           = 0;                                                 //
int                 LeftStateNow            = 0;                                                 //
int                 SelectPreviousState     = 0;                                                 //
int                 RightPreviousState      = 0;                                                 //
int                 LeftPreviousState       = 0;                                                 //
int                 Password                = 0;
int                 MyRotaryRegister[3]     = {0, 0, 0};                                         //
int                 RotaryPrevious[3]       = {0, 0, 0};                                         //
int                 Rotary[3]               = {0, 1, 2};                                         // 
int                 Action[3]               = {0, 0, 0};                                         //
const unsigned int  Password1[4]            = {3, 7, 2, 5};                                      //
const unsigned int  Password2[4]            = {7, 0, 3, 4};                                      //
unsigned int        Digit[4]                = {0, 0, 0, 0};                                      //
unsigned long       DebounceDelay           = 20;                                                // Debounce delay for toggle switches
unsigned long       DebounceTimer           = 0;                                                 // Stores the current value of millis()   
uint8_t             data[4]                 = {0xff, 0xff, 0xff, 0xff };                         //                                                           //
const uint8_t       SEG_SWIRL1[]            = {SEG_A, SEG_A, 0, 0};
const uint8_t       SEG_SWIRL2[]            = {0, 0, SEG_A, SEG_A};
const uint8_t       SEG_SWIRL3[]            = {0 ,0 ,0 ,SEG_B | SEG_C};
const uint8_t       SEG_SWIRL4[]            = {0, 0, SEG_D, SEG_D};
const uint8_t       SEG_SWIRL5[]            = {SEG_D, SEG_D, 0, 0};
const uint8_t       SEG_SWIRL6[]            = {SEG_E | SEG_F, 0, 0, 0};
const uint8_t       SEG_DONE[]              = {SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, SEG_C | SEG_E | SEG_G, SEG_A | SEG_D | SEG_E | SEG_F | SEG_G};
const uint8_t       SEG_PASS[]              = {SEG_A | SEG_B | SEG_E | SEG_F | SEG_G , SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G , SEG_A | SEG_C | SEG_D | SEG_G | SEG_F , SEG_A | SEG_C | SEG_D | SEG_G | SEG_F};
const uint8_t       SEG_FAIL[]              = {SEG_A | SEG_F | SEG_E | SEG_G , SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G , SEG_E | SEG_F, SEG_F | SEG_E | SEG_D};
const uint8_t       SEG_BLINK[]             = {SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G};
TM1637Display       display(CLK, DIO);



void DisplaySwirl()
{
 for (int u = 0; u < 3; u++)
  {
   display.setSegments(SEG_SWIRL1);
   delay(250);
   display.setSegments(SEG_SWIRL2);
   delay(250);
   display.setSegments(SEG_SWIRL3);
   delay(250);
   display.setSegments(SEG_SWIRL4);
   delay(250);
   display.setSegments(SEG_SWIRL5);
   delay(250);
   display.setSegments(SEG_SWIRL6);
   delay(250);
  }

}

void DisplayBlink()
{
  for (int k = 0; k < 5; k++)
   {
     display.setSegments(SEG_BLINK);
     delay(300);
     display.clear();
     delay(200);
    }
}

void EnterPassword()
{
   if ((Password == 0 && (Digit[0] != Password1[0] || Digit[1] != Password1[1] || Digit[2] != Password1[2] || Digit[3] != Password1[3])) || (Password == 1 && (Digit[0] != Password2[0] || Digit[1] != Password2[1] || Digit[2] != Password2[2] || Digit[3] != Password2[3]))) 
     {
        Attempts = Attempts - 1;
        Serial.print("Wrong password, you have ");
        Serial.print((Attempts - 1));
        Serial.println(" attempt(s) left.");
        DisplaySwirl();
        display.setSegments(SEG_FAIL);
        delay(3000);
        memset(Digit, 0, sizeof(Digit));
        memset(data, 0, sizeof(data));
        display.clear();
        SelectDigit = 0;
      }

   if (Password == 0 && (Digit[0] == Password1[0] && Digit[1] == Password1[1] && Digit[2] == Password1[2] && Digit[3] == Password1[3]))
     {
        DisplaySwirl();
        Serial.println("Correct. Step one of two compeleted.");
        display.setSegments(SEG_PASS);
        delay(3000);
        Password = Password +1;
        memset(Digit, 0, sizeof(Digit));
        memset(data, 0, sizeof(data));
        display.clear();
        SelectDigit = 0;
      }
  if (Password == 1 && (Digit[0] == Password2[0] && Digit[1] == Password2[1] && Digit[2] == Password2[2] && Digit[3] == Password2[3]))
      {
        Serial.println("Bomb has been defused");
        DisplaySwirl();
        display.setSegments(SEG_PASS);
        delay(2000);
        DisplayBlink();
        display.setSegments(SEG_DONE);
        delay(1000000);
      }
}

void DebounceSelect()
{
  SelectStateNow = digitalRead(RotaryNC_Pin);
  if(SelectStateNow != SelectPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(SelectStateNow != RotarySelect)
    {
      RotarySelect = SelectStateNow;
      if (RotarySelect == LOW && digitalRead(RotaryNC_Pin) == LOW )
        {
          MyRotaryRegister[0] = 1;   // ON
        }
      if (RotarySelect == HIGH) 
        {
          MyRotaryRegister[0] = 0;   // OFF
        }
    }
  }
  SelectPreviousState = SelectStateNow; 
}

void DebounceRight()
{
  RightStateNow = digitalRead(RotarySW1_Pin);
  if(RightStateNow != RightPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(RightStateNow != RotaryRight)
    {
      RotaryRight = RightStateNow;
      if (RotaryRight == LOW && digitalRead(RotarySW1_Pin) == LOW)
        {
          MyRotaryRegister[1] = 1;   // ON
        }
      if (RotaryRight == HIGH) 
        {
          MyRotaryRegister[1] = 0;   // OFF
        }
    }
  }
  RightPreviousState = RightStateNow; 
}

void DebounceLeft()
{
  LeftStateNow = digitalRead(RotarySW2_Pin);
  if(LeftStateNow != LeftPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(LeftStateNow != RotaryLeft)
    {
      RotaryLeft = LeftStateNow;
      if (RotaryLeft == LOW && digitalRead(RotarySW2_Pin) == LOW)
        {
          MyRotaryRegister[2] = 1;   // ON
        }
      if (RotaryLeft == HIGH) 
        {
          MyRotaryRegister[2] = 0;   // OFF
        }
    }
  }
  LeftPreviousState = LeftStateNow; 
}




void setup()
{
Serial.begin(9600);
pinMode(RotaryNC_Pin, INPUT_PULLUP);        // Initialize Toggle switch pins as NC
pinMode(RotarySW1_Pin, INPUT_PULLUP);
pinMode(RotarySW2_Pin, INPUT_PULLUP);
display.setBrightness(BRIGHTNESS);
display.clear();
}

void loop()
{

while (Attempts == 0)
{
  Serial.println("Your attempts were unsuccessful, detonation sequence initiated...");
  for (int t = 10; t > 0; t--)
   {
     Serial.print(t);
     Serial.println("...");
     delay(1000);
   }
  Serial.println("Bomb exploded.");
  delay(100000);
}

 DebounceSelect();
 DebounceRight();
 DebounceLeft(); 

 data[0] = display.encodeDigit(Digit[0]);
 data[1] = display.encodeDigit(Digit[1]);
 data[2] = display.encodeDigit(Digit[2]);
 data[3] = display.encodeDigit(Digit[3]);
 display.setSegments(data);
 
 for (int i = 0; i < 3; i++)
 {
   switch(MyRotaryRegister[i])
    {
      case 0:
       RotaryPrevious[i] = 0;
       break;

      case 1:
        if ((RotaryPrevious[i] != MyRotaryRegister[i]) && MyRotaryRegister[i] == 1)   
         {
           RotaryPrevious[i] = 1;
           //Serial.println(Rotary[i]);  // Return value of 0 = select, 1 = Right, 2 = Left 
           Action[i] = 1;
           break;
         }
       default:
           break;
    }  
  }


  if (SelectDigit == 0)
   {
     if (Action[0] == 1)
       {
         SelectDigit++;
         display.clear();
        }
     if (Action[1] == 1)
       {
         Digit[0] = Digit[0] + 1;
         if(Digit[0] > 9){Digit[0] = 9;}
        }
     if (Action[2] == 1)
       {
         if(Digit[0] != 0){Digit[0] = Digit[0] - 1;}
        }
     memset(Action, 0, sizeof(Action));
    }

  if (SelectDigit == 1)
   {
     if (Action[0] == 1)
       {
         SelectDigit++;
         display.clear();
        }
     if (Action[1] == 1)
       {
         Digit[1] = Digit[1] + 1;
         if(Digit[1] > 9){Digit[1] = 9;}
        }
     if (Action[2] == 1)
       {
         if(Digit[1] != 0){Digit[1] = Digit[1] - 1;}
        }
     memset(Action, 0, sizeof(Action));
    }

 if (SelectDigit == 2)
   {
      if (Action[0] == 1)
       {
         SelectDigit++;
         display.clear();
        }
      if (Action[1] == 1)
       {
         Digit[2] = Digit[2] + 1;
         if(Digit[2] > 9){Digit[2] = 9;}
        }
      if (Action[2] == 1)
       {
         if(Digit[2] != 0){Digit[2] = Digit[2] - 1;}
        }
     memset(Action, 0, sizeof(Action));
    }

 if (SelectDigit == 3)
   {
     if (Action[1] == 1)
       {
         Digit[3] = Digit[3] + 1;
         if(Digit[3] > 9){Digit[3] = 9;}
        }
     if (Action[2] == 1)
       {
         if(Digit[3] != 0){Digit[3] = Digit[3] - 1;}
        }
     if (Action[0] == 1)
        {
         display.clear();
         EnterPassword();
        }
     memset(Action, 0, sizeof(Action));
    }
 
}
