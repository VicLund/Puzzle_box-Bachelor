#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>
//#include <DFRobot_I2CMultiplexer.h>

// Variables for the LED gravity buttons for the menu and setting them to a pin
const int menuButtonPgUp = A1;
const int menuButtonPgDn = A2;
const int menuButtonEnter = A3;
const int menuButtonEsc = A4;

// Variables for module buttons, 1 pin for LED, 1 pin for actual button
const int LO_button = 1;
const int LO_LED = 2;
const int RtN_button = 3;
const int RtN_LED = 4;
const int FtS_button = 5;
const int FtS_LED = 6;
const int CtC_button = 7;
const int CtC_LED = 8;

//Variable for toggle switch
const int toggle_switch = 9;

// Current button state and last button state variables for menu buttons
int menuButtonPgUp_state;
int menuButtonPgUp_lastState = LOW;
int menuButtonPgDn_state;
int menuButtonPgDn_lastState = LOW;
int menuButtonEnter_state;
int menuButtonEnter_lastState = LOW;
int menuButtonEsc_state;
int menuButtonEsc_lastState = LOW;
int toggleSwitch_state = 0;
int toggleSwitch_lastState = LOW;

// Current button state and last button state variablea for module buttons
int LO_buttonState;
int LO_lastButtonState = LOW;
int RtN_buttonState;
int RtN_lastButtonState = LOW;
int FtS_buttonState;
int FtS_lastButtonState = LOW;
int CtC_buttonState;
int CtC_lastButtonState = LOW;

// Declaring debounce variables
unsigned long debounceDelay = 50;
unsigned long longPressDelay = 3000;

unsigned long lastDebounceTimeMenu = 0;
unsigned long lastDebounceTimeModule = 0;
unsigned long lastDebounceTimeToggle = 0;

// Boolean variables for checking if the light is on or off on the button
boolean LO_LED_on = false;
boolean RtN_LED_on = false;
boolean FtS_LED_on = false;
boolean CtC_LED_on = false;

// Set the LCD address to 0x20 for a 16x02 display
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Menu variable that always starts at 1
int mainMenu = 1;

// Define the Watchdog Interrupt register manually
#define WDT_CTRL *(uint8_t*) (0x40001000+0x00)

// Long and short button press variables (only used for Esc button)
boolean shortPressActive = false;
boolean longPressActive = false;

//Forward delaring functions
void debounceMenuButtonPgUp();
void debounceMenuButtonPgDn();
void debounceMenuButtonEnter();
void debounceMenuButtonEsc();
  
void debounce_LO_Button();
void debounce_RtN_Button();
void debounce_FtS_Button();
void debounce_CtC_Button();

void debounceToggleSwitch();

void updateMainMenu();
void executeReset();
void executeMainMenuAction();

void singleModule();
void playGame();

void setup() {
  Serial.begin(9600);
  while(!Serial){
    delay(10);
  }

  lcd.init();
  lcd.backlight();

  //Setting the pin mode for all buttons
  pinMode(menuButtonPgUp, INPUT_PULLUP);
  pinMode(menuButtonPgDn, INPUT_PULLUP);
  pinMode(menuButtonEnter, INPUT_PULLUP);
  pinMode(menuButtonEsc, INPUT_PULLUP);

  pinMode(LO_button, INPUT_PULLUP);
  pinMode(LO_LED, OUTPUT);
  pinMode(RtN_button, INPUT_PULLUP);
  pinMode(RtN_LED, OUTPUT);
  pinMode(FtS_button, INPUT_PULLUP);
  pinMode(FtS_LED, OUTPUT);
  pinMode(CtC_button, INPUT_PULLUP);
  pinMode(CtC_LED, OUTPUT);

  pinMode(toggle_switch, INPUT_PULLDOWN);
}

void loop() {
  //Running the button functions in the main loop function so the program always "listens" for button presses
  debounceMenuButtonPgUp();
  debounceMenuButtonPgDn();
  debounceMenuButtonEnter();
  debounceMenuButtonEsc();
  
  debounce_LO_Button();
  debounce_RtN_Button();
  debounce_FtS_Button();
  debounce_CtC_Button();

}

void updateMainMenu(){
  switch(mainMenu){
    case 0:
      mainMenu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print("> Play 1 module");
      lcd.setCursor(0,1);
      lcd.print("  Play game");
      break;
    case 2:
      lcd.clear();
      lcd.print("  Play 1 module");
      lcd.setCursor(0,1);
      lcd.print("> Play game");
    case 3:
      mainMenu = 2;
      break;
  }
}

void executeReset(){
  WDT_CTRL = WDT_CTRL_ENABLE;
}

void executeMainMenuAction(){
  switch(mainMenu){
    case 1:
      singleModule();
      break;
    case 2:
      playGame();
      break;
  }
}

// Debounce function for Menu button "Page Up"
void debounceMenuButtonPgUp(){
  int menuButtonPgUp_reading = digitalRead(menuButtonPgUp);

  if (menuButtonPgUp_reading != menuButtonPgUp_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonPgUp_reading != menuButtonPgUp_state){
      menuButtonPgUp_state = menuButtonPgUp_reading;
      if (menuButtonPgUp_state == LOW){
        Serial.println("Page Up button has been pressed");
        mainMenu--;
        updateMainMenu();
        delay(100);
      }
    }
  }
  menuButtonPgUp_lastState = menuButtonPgUp_reading;
}

// Debounce function for Menu button "Page Down"
void debounceMenuButtonPgDn(){
  int menuButtonPgDn_reading = digitalRead(menuButtonPgDn);
  
  if (menuButtonPgDn_reading != menuButtonPgDn_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonPgDn_reading != menuButtonPgDn_state){
      menuButtonPgDn_state = menuButtonPgDn_reading;
      if (menuButtonPgDn_state == LOW){
        Serial.println("Page Down button has been pressed");
        mainMenu++;
        updateMainMenu();
        delay(100);
      }
    }
  }

  menuButtonPgDn_lastState = menuButtonPgDn_reading;
}

// Debounce function for Menu button "Enter"
void debounceMenuButtonEnter(){
  int menuButtonEnter_reading = digitalRead(menuButtonEnter);

  if (menuButtonEnter_reading != menuButtonEnter_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonEnter_reading != menuButtonEnter_state){
      menuButtonEnter_state = menuButtonEnter_reading;
      if (menuButtonEnter_state == LOW){
        Serial.println("Enter button has been pressed");
        executeMainMenuAction();
        updateMainMenu();
        delay(100);
      }
    }
  }
  menuButtonEnter_lastState = menuButtonEnter_reading;
}

// Debounce function for Menu button "Escape"
/*void debounceMenuButtonEsc(){
  int menuButtonEsc_reading = digitalRead(menuButtonEsc);
  
  if (menuButtonEsc_reading != menuButtonEsc_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonEsc_reading != menuButtonEsc_state){
      menuButtonEsc_state = menuButtonEsc_reading;
      if (menuButtonEsc_state == LOW){
        Serial.println("Escape button has been pressed");
        executeReset();
        updateMenu();
        delay(100);
      }
    }
  }

  menuButtonEsc_lastState = menuButtonEsc_reading;
}*/

void debounceMenuButtonEsc(){
  if (digitalRead(menuButtonEsc) == HIGH) {
    if (shortPressActive == false) {
      shortPressActive = true;
      lastDebounceTimeMenu = millis();
    }
    if ((millis() - lastDebounceTimeMenu > longPressDelay) && (longPressActive == false)) {
      longPressActive = true;
      //Serial.println("Long press");
      executeReset();
      //resetFunc();
    }
  } else {
    if (shortPressActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        //Serial.println("Short press");
        updateMainMenu();
      }
      shortPressActive = false;
    }
  }
}

void LO_buttonLED_status(){
  if(LO_LED_on){
    LO_LED_on = false;
  }
  else{
    LO_LED_on = true;
  }
}

void RtN_buttonLED_status(){
  if(RtN_LED_on){
    RtN_LED_on = false;
  }
  else{
    RtN_LED_on = true;
  }
}

void FtS_buttonLED_status(){
  if(FtS_LED_on){
    FtS_LED_on = false;
  }
  else{
    FtS_LED_on = true;
  }
}

void CtC_buttonLED_status(){
  if(CtC_LED_on){
    CtC_LED_on = false;
  }
  else{
    CtC_LED_on = true;
  }
}

// Debounce function for module button "Lights Out"
void debounce_LO_Button(){
  int LO_reading = digitalRead(LO_button);

  if (LO_reading != LO_lastButtonState){
    lastDebounceTimeModule = millis();
  }

  if ((millis() - lastDebounceTimeModule) > debounceDelay){
    if (LO_reading != LO_buttonState){
      LO_buttonState = LO_reading;
      if (LO_buttonState == LOW){
        LO_buttonLED_status();
        if(LO_LED_on){
          digitalWrite(LO_LED, HIGH);
          Serial.println("\"Lights Out\" module has been chosen");
          Serial.println("LO button light on");
        }
        else{
          digitalWrite(LO_LED, LOW);
          Serial.println("LO button light off");
        }
      }
    }
  }

  LO_lastButtonState = LO_reading;
}

// Debounce function for module button "Recognize the Note"
void debounce_RtN_Button(){
    int RtN_reading = digitalRead(RtN_button);

  if (RtN_reading != RtN_lastButtonState){
    lastDebounceTimeModule = millis();
  }

  if ((millis() - lastDebounceTimeModule) > debounceDelay){
    if (RtN_reading != RtN_buttonState){
      RtN_buttonState = RtN_reading;
      if (RtN_buttonState == LOW){
        RtN_buttonLED_status();
        if(RtN_LED_on){
          digitalWrite(RtN_LED, HIGH);
          Serial.println("\"Recognize the Note\" module has been chosen");
          Serial.println("RtN button light on");
        }
        else{
          digitalWrite(RtN_LED, LOW);
          Serial.println("RtN button light off");
        }
      }
    }
  }

  RtN_lastButtonState = RtN_reading;
}

// Debounce function for module button "Flip the Switch"
void debounce_FtS_Button(){
    int FtS_reading = digitalRead(FtS_button);

  if (FtS_reading != FtS_lastButtonState){
    lastDebounceTimeModule = millis();
  }

  if ((millis() - lastDebounceTimeModule) > debounceDelay){
    if (FtS_reading != FtS_buttonState){
      FtS_buttonState = FtS_reading;
      if (FtS_buttonState == LOW){
        FtS_buttonLED_status();
        if(FtS_LED_on){
          digitalWrite(FtS_LED, HIGH);
          Serial.println("\"Flip the Switch\" module has been chosen");
          Serial.println("FtS button light on");
        }
        else{
          digitalWrite(FtS_LED, LOW);
          Serial.println("Fts button light off");
        }
      }
    }
  }

  FtS_lastButtonState = FtS_reading;
}

// Debounce function for module button "Crack the Code"
void debounce_CtC_Button(){
    int CtC_reading = digitalRead(CtC_button);

  if (CtC_reading != CtC_lastButtonState){
    lastDebounceTimeModule = millis();
  }

  if ((millis() - lastDebounceTimeModule) > debounceDelay){
    if (CtC_reading != CtC_buttonState){
      CtC_buttonState = CtC_reading;
      if (CtC_buttonState == LOW){
        CtC_buttonLED_status();
        if(CtC_LED_on){
          digitalWrite(CtC_LED, HIGH);
          Serial.println("\"Crack the Code\" module has been chosen");
          Serial.println("CtC button light on");
        }
        else{
          digitalWrite(CtC_LED, LOW);
          Serial.println("CtC button light off");
        }
      }
    }
  }

  CtC_lastButtonState = CtC_reading;
}

void debounceToggleSwitch(){
  int toggleSwitch_reading = digitalRead(toggle_switch);

  if (toggleSwitch_reading != toggleSwitch_lastState){
    lastDebounceTimeToggle = millis();
  }

  if ((millis() - lastDebounceTimeToggle) > debounceDelay){
    if (toggleSwitch_reading != toggleSwitch_state){
      toggleSwitch_state = toggleSwitch_reading;
      if (toggleSwitch_state == LOW){
        Serial.println("The bomb has been disarmed");
      }
      if (toggleSwitch_state == HIGH){
        Serial.println("Bomb has been armed");
      }
    }
  }
  toggleSwitch_lastState = toggleSwitch_reading;
}

void singleModule(){

}

void playGame(){

}