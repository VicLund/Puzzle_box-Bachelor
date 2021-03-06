#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>


const int RedLEDPin = 5;
const int RedPButtonStatePin = 4;
int RedPButtonState = LOW;
int RedPButtonPreviousState = LOW;
boolean on = false;

unsigned long DebounceDelay = 50;
unsigned long DebounceTimer = 0;


void setup() {
Serial.begin(9600);
pinMode(RedLEDPin, OUTPUT);
pinMode(RedPButtonStatePin, INPUT);
}

void loop() {

//Solution without DEBOUNCE
//Button acts as a Switch, LIGHTS On/Off when Pressed

  RedPButtonState = digitalRead(RedPButtonStatePin);
  if (RedPButtonState == HIGH) {    
    if(on==true){
      on=false;
    }  else{
      on=true;
    }
  }
 
  
   if (on == true) {    
    digitalWrite(RedLEDPin, HIGH);
    Serial.println("Red PushButton ON");  
  }
  else {
    digitalWrite(RedLEDPin, LOW);
    Serial.println("Red PushButton OFF");
  }
  
   delay(500); 

}

// This debounce (should**) works but only if 'INPUT' in pinMode() is changed to 'INPUT_PULLUP'
//**I haven't actually tested it, but it's almost identical to what I've written for my push buttons
/*void DebouncePushButtonRed()
{
  int RedPBStateNow = digitalRead(RedPButtonPin);
  if(RedPBStateNow != RedPButtonPreviousState)
  {
    DebounceTimer = millis();
  }
  if ((millis() - DebounceTimer) > DebounceDelay)
  {
    if(RedPBStateNow != RedPButtonState)
    {
      RedPButtonState = RedPBStateNow;
      if (RedPButtonState == LOW)
      {
        RedButton_status();
        if(on)
        {
          digitalWrite(RedLEDPin, HIGH);
          Serial.println("Red PushButton ON");
        }
        else
        {
          digitalWrite(RedLEDPin, LOW);
          Serial.println("Red PushButton OFF");
        }
      }
    }
  }
  RedPButtonPreviousState = RedPBStateNow; 
}

void RedButton_status()
{
  if(on)
  {
    on = false;
  }
  else
  {
    on = true;
  }
}*/