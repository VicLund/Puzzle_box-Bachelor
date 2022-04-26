#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <DFRobotDFPlayerMini.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_seesaw.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoTrellis.h>
//#include <DFRobot_I2CMultiplexer.h>

//I2C slaves defines
#define slaveAdr1 0x01
#define slaveAdr2 0x02

//Define variable to send to slaves
int RtN = 1;
int FtS = 2;
int CtC = 3;

int start = 6;
int boom = 7;
int finish = 8;
//int WDT_reset = 9;

//Define a variable to receive
byte masterReceiveSlave1;
byte masterReceiveSlave2;

// Variables for the LED gravity buttons for the menu and setting them to a pin
const int menuButtonPgUp = A0;
const int menuButtonPgDn = A1;
const int menuButtonEnter = A2;
const int menuButtonEsc = A3;

// Variables for module buttons, 1 pin for LED, 1 pin for actual button
const int LO_button = A5;
const int LO_LED = A6;
const int RtN_button = 0;
const int RtN_LED = 1;
const int FtS_button = 2;
const int FtS_LED = 3;
const int CtC_button = 4;
const int CtC_LED = 5;

//Variable for toggle switch
const int toggle_switch = 8;

//Variables for LED strip
#define LEDpin 6
#define numStripPixels 92
int brightness = 50;

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
//unsigned long longPressDelay = 3000;

unsigned long lastDebounceTimeMenu = 0;
unsigned long lastDebounceTimeModule = 0;
unsigned long lastDebounceTimeToggle = 0;

// Countdown timer variables
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long interval = 1000;
int totalSeconds = 10;

// Boolean variables for checking if the light is on or off on the button
boolean LO_LED_on = false;
boolean RtN_LED_on = false;
boolean FtS_LED_on = false;
boolean CtC_LED_on = false;

// Set the LCD address to 0x20 for a 16x02 display
LiquidCrystal_I2C lcd(0x20, 16, 2);

//Set up LED strip, with # of pixels, pin #, and type
Adafruit_NeoPixel strip(numStripPixels, LEDpin, NEO_GRB + NEO_KHZ800);

// Menu variable that always starts at 1
int mainMenu = 1;

//These 5 arrays paint the bars that go across the screen.  
byte zero[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
byte one[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
byte two[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
byte three[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
byte four[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};
byte five[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

/*// Define the Watchdog Interrupt register manually
#define WDT_CTRL *(uint8_t*) (0x40001000+0x00)

// Long and short button press variables (only used for Esc button)
boolean shortPressActive = false;
boolean longPressActive = false;*/

// Boolean for if a module has been chosen or not
boolean moduleChosen = false;

// Boolean for checking if a singlePlay is chosen or if gamePlay is chosen
boolean playGame = false;

// Boolean for checking if a module is actually finished
boolean moduleFinished = false;

// Boolean for if toggle switch is on or off
boolean toggleState = false;

// Boolean for checking if it's ok to flip toggle switch
boolean timeForToggleOn = true;
boolean timeForToggleOff = false;
boolean countdownTimerOn = false;

// Play/try again booleans
boolean waitPlayAgain = false;
boolean askPlayAgain = false;

//Lights On variables
Adafruit_NeoTrellis trellis;

int brightness = 20;
int buttonPressed;
int buttonAdjUp;
int buttonAdjRight;
int buttonAdjDown;
int buttonAdjLeft;

uint32_t buttonPressedColor;
uint32_t buttonAdjUpColor;
uint32_t buttonAdjRightColor;
uint32_t buttonAdjDownColor;
uint32_t buttonAdjLeftColor;

uint32_t noLight = 0x000000;
uint32_t red = 0xFF0000;
uint32_t green = 0x00FF00;
uint32_t blue = 0x0000FF;
uint32_t yellow = 0xFFF200;
uint32_t purple = 0xD900FF;

int regularButtonsArray[] = {0, 1, 2, 5, 6, 9, 10, 13, 14, 15};
int leftButtonsArray[] = {3, 7, 11};
int rightButtonsArray[] = {4, 8, 12};

int regularButtonsArraySize = sizeof(regularButtonsArray) / sizeof(int);
int leftButtonsArraySize = sizeof(leftButtonsArray) / sizeof(int);
int rightButtonsArraySize = sizeof(rightButtonsArray) / sizeof(int);

int trellisArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int trellisArraySize = sizeof(trellisArray) / sizeof(int);

int trellisSum = 0;

int animateRBArray[] = {0, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4, 5, 6, 10, 9};
int animateRBArraySize = sizeof(animateRBArray) / sizeof(int);

uint32_t fireworksColors[] = {blue, noLight, purple, noLight, red, noLight, yellow, noLight, green, noLight};
int fireworksColorsSize = sizeof(fireworksColors) / sizeof(uint32_t);

int cheatCodeNr = 0;
boolean LO_finished = false;

//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    buttonPressed = evt.bit.NUM;
    LO_buttonCheck(buttonPressed);
    checkFinished();
    cheatCode(buttonPressed);
    
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
  // or is the pad released?
  }

  // Turn on/off the neopixels!
  trellis.pixels.show();

  return 0;
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

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

void startSequence();
void startSequence_switchOn();
void endSequence();
void trickEnding();
void properEnding();

void updateMainMenu();
void executeReset();
void startUpLights();
void executeMainMenuAction();

void singlePlay();
void gamePlay();

void LO_firstModule_gamePlay();
void RtN_firstModule_gamePlay();
void FtS_firstModule_gamePlay();
void CtC_firstModule_gamePlay();

void LO_firstModule_singlePlay();
void RtN_firstModule_singlePlay();
void FtS_firstModule_singlePlay();
void CtC_firstModule_singlePlay();

void noSignal_slave1();
void noSignal_slave2();
void requestSlave1();
void requestSlave2();

void LO_func();
void RtN_func();
void FtS_func();
void CtC_func();

void updateProgressBar(unsigned long, unsigned long, int);
void progressBar_startStopPoint(int, int);

//Lights On
void trellisStartup();
void trellisShutDown();
void trellisStartupAnimation();
void checkFinished();
void finishSequence();
void emptyTrellisArray();
void cheatCode(int);
void LO_buttonCheck(int);
void regularAdjButtons(int);
void leftAdjButtons(int);
void rightAdjButtons(int);
void rainbowChaseBorder();
void fireworks();
void rainbowPulse();
void blinkRed();

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  //updateMainMenu();

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(brightness); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  startSequence();

  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);

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
  debounceToggleSwitch();
}

void startSequence(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("Flip the switch");

  //Show red light behind toggle switch
  strip.clear();
  strip.show();
  strip.fill(strip.Color(255, 0, 0), 20, 28);
  strip.show();
}

void startSequence_switchOn(){
  lcd.noAutoscroll();
  Wire.beginTransmission(slaveAdr1);
  Wire.write(start);
  Wire.endTransmission();

  updateMainMenu();
  startUpLights();
}

void endSequence(){
  timeForToggleOff = true;
  lcd.print("Uh Oh!");
  while(countdownTimerOn){
    currentTime = millis();
    if(currentTime - previousTime > interval){
      previousTime = currentTime;
      if(totalSeconds != 0){
        totalSeconds--;
        lcd.setCursor(0, 7);
        lcd.print(totalSeconds);
        debounceToggleSwitch();
      }
      debounceToggleSwitch();
    }
    debounceToggleSwitch();
  }

}

void properEnding(){
  countdownTimerOn = false;
  timeForToggleOff = true;
  askPlayAgain = true;
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Congrats!!");
  lcd.setCursor(4, 1);
  lcd.print("You won!");
  //Flashy lights
  //Winning song?
  delay(3000);
  lcd.clear();
  lcd.print("Play again?");
  while(waitPlayAgain){
    debounceMenuButtonEnter();
  }
}

void trickEnding(){
  countdownTimerOn = false;
  timeForToggleOff = true;
  askPlayAgain = true;
  lcd.clear();
  lcd.print("Boom.");
  //Send boom sound
  //Make lights "explode"
  delay(5000);
  lcd.clear();
  lcd.print("Game Over");
  lcd.setCursor(0,1);
  lcd.print("You fell for it");
  delay(3000);
  lcd.clear();
  lcd.print("Try again?");
  while(waitPlayAgain){
    debounceMenuButtonEnter();
  }
}

void startUpLights(){
  strip.clear();
  strip.show();
  for(int i = 0; i < numStripPixels; i++){
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(30);
  }
  for(int j = brightness; j >= 0; j--){
    strip.setBrightness(j);
    strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
    strip.show();
    delay(20);
  }
  for(int k = 0; k < brightness; k++){
    strip.setBrightness(k);
    strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
    strip.show();
    delay(20);
  }
}

/*void executeReset(){
  WDT_CTRL = WDT_CTRL_ENABLE;
}*/

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
      break;
    case 3:
      mainMenu = 2;
      break;
  }
}


void executeMainMenuAction(){
  switch(mainMenu){
    case 1:
      singlePlay();
      break;
    case 2:
      gamePlay();
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
        if(toggleState == true){
          Serial.println("Page Up button has been pressed");
          mainMenu--;
          updateMainMenu();
          delay(100);
        }
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
        if(toggleState == true){
          Serial.println("Page Down button has been pressed");
          mainMenu++;
          updateMainMenu();
          delay(100);
        }
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
        if(toggleState == true){
          Serial.println("Enter button has been pressed");
          executeMainMenuAction();
          updateMainMenu();
          delay(100);
          if(askPlayAgain == true){
            startSequence();
          }
        }
      }
    }
  }
  menuButtonEnter_lastState = menuButtonEnter_reading;
}

// Debounce function for Menu button "Escape"
void debounceMenuButtonEsc(){
  int menuButtonEsc_reading = digitalRead(menuButtonEsc);
  
  if (menuButtonEsc_reading != menuButtonEsc_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonEsc_reading != menuButtonEsc_state){
      menuButtonEsc_state = menuButtonEsc_reading;
      if (menuButtonEsc_state == LOW){
        if(toggleState == true){
          Serial.println("Escape button has been pressed");
          moduleChosen = true;
          playGame = false;
          digitalWrite(LO_LED, LOW);
          digitalWrite(RtN_LED, LOW);
          digitalWrite(FtS_LED, LOW);
          digitalWrite(CtC_LED, LOW); 
          updateMainMenu();
          delay(100);
        }
      }
    }
  }

  menuButtonEsc_lastState = menuButtonEsc_reading;
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
        moduleChosen = true;
        
        Serial.println("\"Lights Out\" module has been chosen");
        
        digitalWrite(LO_LED, LOW);
        digitalWrite(RtN_LED, LOW);
        digitalWrite(FtS_LED, LOW);
        digitalWrite(CtC_LED, LOW);
        strip.clear();
        strip.show();
        if (playGame){
          LO_firstModule_gamePlay();
        }else{
          LO_firstModule_singlePlay();
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
        moduleChosen = true;
        
        Serial.println("\"Recognize the Note\" module has been chosen");
        
        digitalWrite(LO_LED, LOW);
        digitalWrite(RtN_LED, LOW);
        digitalWrite(FtS_LED, LOW);
        digitalWrite(CtC_LED, LOW);
        strip.clear();
        strip.show();
        if (playGame){
          RtN_firstModule_gamePlay();
        }else{
          RtN_firstModule_singlePlay();
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
        moduleChosen = true;
        
        Serial.println("\"Flip the Switch\" module has been chosen");
        
        digitalWrite(LO_LED, LOW);
        digitalWrite(RtN_LED, LOW);
        digitalWrite(FtS_LED, LOW);
        digitalWrite(CtC_LED, LOW);
        strip.clear();
        strip.show();
        if (playGame){
          FtS_firstModule_gamePlay();
        }else{
          FtS_firstModule_singlePlay();
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
        moduleChosen = true;
        
        Serial.println("\"Crack the Code\" module has been chosen");
        
        digitalWrite(LO_LED, LOW);
        digitalWrite(RtN_LED, LOW);
        digitalWrite(FtS_LED, LOW);
        digitalWrite(CtC_LED, LOW);
        strip.clear();
        strip.show();
        if (playGame){
          CtC_firstModule_gamePlay();
        }else{
          CtC_firstModule_singlePlay();
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
        if(timeForToggleOff == true){
          if(totalSeconds == 0){
            toggleState = false;
            timeForToggleOff = false;
            timeForToggleOn = true;
            waitPlayAgain = true;
            properEnding();
          }
          else{
            toggleState = false;
            timeForToggleOff = false;
            timeForToggleOn = true;
            waitPlayAgain = true;
            trickEnding();
          }
          //Serial.println("The bomb has been disarmed");
        }
      }
      if (toggleSwitch_state == HIGH){
        if(timeForToggleOn == true){
          Serial.println("Bomb has been armed");
          toggleState = true;
          timeForToggleOn = false;
          startSequence_switchOn();
        }
      }
    }
  }
  toggleSwitch_lastState = toggleSwitch_reading;
}

void singlePlay(){
  lcd.clear();
  lcd.print("Pick a module");
  lcd.setCursor(0,1);
  lcd.print("Esc to exit");

  digitalWrite(LO_LED, HIGH);
  digitalWrite(RtN_LED, HIGH);
  digitalWrite(FtS_LED, HIGH);
  digitalWrite(CtC_LED, HIGH);
  
  while(!moduleChosen){
    debounce_LO_Button();
    debounce_RtN_Button();
    debounce_FtS_Button();
    debounce_CtC_Button();
    debounceMenuButtonEsc();
  }
  moduleChosen = false;
}

void gamePlay(){
  playGame = true;
  lcd.clear();
  lcd.print("Pick 1st module");
  lcd.setCursor(0,1);
  lcd.print("Esc to exit");

  digitalWrite(LO_LED, HIGH);
  digitalWrite(RtN_LED, HIGH);
  digitalWrite(FtS_LED, HIGH);
  digitalWrite(CtC_LED, HIGH);
  
  while(!moduleChosen){
    debounce_LO_Button();
    debounce_RtN_Button();
    debounce_FtS_Button();
    debounce_CtC_Button();
    debounceMenuButtonEsc();
  }
  moduleChosen = false;
}

void LO_firstModule_singlePlay(){
  lcd.clear();
  lcd.print("Running module:");
  lcd.setCursor(0,1);
  lcd.print("Lights On");
  LO_func();
  lcd.clear();
  lcd.print("Congratulations!");
  lcd.setCursor(0,1);
  lcd.print("Module Complete");
  delay(3000);
  strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
  strip.show();
  updateMainMenu();
}

void RtN_firstModule_singlePlay(){
  lcd.clear();
  lcd.print("Running module:");
  lcd.setCursor(0,1);
  lcd.print("Rec. the Note");
  RtN_func();
  lcd.clear();
  lcd.print("Congratulations!");
  lcd.setCursor(0,1);
  lcd.print("Module Complete");
  delay(3000);
  strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
  strip.show();
  updateMainMenu();
}

void FtS_firstModule_singlePlay(){
  lcd.clear();
  lcd.print("Running module:");
  lcd.setCursor(0,1);
  lcd.print("Flip the Switch");
  FtS_func();
  lcd.clear();
  lcd.print("Congratulations!");
  lcd.setCursor(0,1);
  lcd.print("Module Complete");
  delay(3000);
  strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
  strip.show();
  updateMainMenu();
}

void CtC_firstModule_singlePlay(){
  lcd.clear();
  lcd.print("Running module:");
  lcd.setCursor(0,1);
  lcd.print("Crack the Code");
  CtC_func();
  lcd.clear();
  lcd.print("Congratulations!");
  lcd.setCursor(0,1);
  lcd.print("Module Complete");
  delay(3000);
  strip.fill(strip.Color(255, 255, 255), 0, numStripPixels);
  strip.show();
  updateMainMenu();
}

void LO_firstModule_gamePlay(){
  lcd.clear();
  lcd.print("0 %");
  LO_func();
  progressBar_startStopPoint(0, 25);
  FtS_func();
  progressBar_startStopPoint(25, 50);
  CtC_func();
  progressBar_startStopPoint(50, 75);
  RtN_func();
  progressBar_startStopPoint(75, 100);
  playGame = false;
  countdownTimerOn = true;
  endSequence();
}

void RtN_firstModule_gamePlay(){
  lcd.clear();
  lcd.print("0 %");
  RtN_func();
  progressBar_startStopPoint(0, 25);
  LO_func();
  progressBar_startStopPoint(25, 50);
  FtS_func();
  progressBar_startStopPoint(50, 75);
  CtC_func();
  progressBar_startStopPoint(75, 100);
  playGame = false;
  countdownTimerOn = true;
  endSequence();
}

void FtS_firstModule_gamePlay(){
  lcd.clear();
  lcd.print("0 %");
  FtS_func();
  progressBar_startStopPoint(0, 25);
  CtC_func();
  progressBar_startStopPoint(25, 50);
  RtN_func();
  progressBar_startStopPoint(50, 75);
  LO_func();
  progressBar_startStopPoint(75, 100);
  playGame = false;
  countdownTimerOn = true;
  endSequence();
}

void CtC_firstModule_gamePlay(){
  lcd.clear();
  lcd.print("0 %");
  CtC_func();
  progressBar_startStopPoint(0, 25);
  RtN_func();
  progressBar_startStopPoint(25, 50);
  LO_func();
  progressBar_startStopPoint(50, 75);
  FtS_func();
  progressBar_startStopPoint(75, 100);
  playGame = false;
  countdownTimerOn = true;
  endSequence();
}

void LO_func(){
  moduleChosen = true;
  Serial.println("LO function running");
  trellisStartup();
  trellisStartupAnimation();
  while(!LO_finished){
    trellis.read();
  }
  LO_finished = false;
  //delay(2000);
}

void RtN_func(){
  moduleChosen = true;
  Serial.println("RtN function running");
  delay(100);
  Wire.beginTransmission(slaveAdr1);
  Wire.write(RtN);
  Wire.endTransmission();
  while(!moduleFinished){
    requestSlave1();
  }
  moduleFinished = false;
}

void FtS_func(){
  moduleChosen = true;
  Serial.println("FtS function running");
  delay(100);
  Wire.beginTransmission(slaveAdr2);
  Wire.write(FtS);
  Wire.endTransmission();
  while(!moduleFinished){
    requestSlave2();
  }
  moduleFinished = false;
}

void CtC_func(){
  moduleChosen = true;
  Serial.println("CtC function running");
  delay(100);
  Wire.beginTransmission(slaveAdr2);
  Wire.write(CtC);
  Wire.endTransmission();
  requestSlave2();
}

void requestSlave1(){
  Wire.requestFrom(slaveAdr1, 1);
  masterReceiveSlave1 = Wire.read();
  if(masterReceiveSlave1 == 1){
    Serial.println("RtN module done!");
    moduleFinished = true;
  }
  delay(2000);
}

void requestSlave2(){
  Wire.requestFrom(slaveAdr2, 1);
  masterReceiveSlave2 = Wire.read();
  /*if(masterReceiveSlave2 == 0){
    noSignal_slave2(); 
  }*/
  if(masterReceiveSlave2 == 2){
    Serial.println("FtS module done!");
  }
  else if(masterReceiveSlave2 == 3){
    Serial.println("CtC module done!");
  }
  delay(2000);
}

void noSignal_slave1(){
  requestSlave1();
}

void noSignal_slave2(){
  requestSlave2();
}

void progressBar_startStopPoint(int pb_startPoint, int pb_stopPoint){
  for(int i = pb_startPoint; i <= pb_stopPoint; i++){
    lcd.setCursor(0,0);
    lcd.print(i);
    lcd.print("%"); 
    lcd.print("   ");
    updateProgressBar(i, 100, 1);
    delay(100);
  } 
}

// Not own code, taken from: https://www.instructables.com/Simple-Progress-Bar-for-Arduino-and-LCD/
 void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
 {
    double factor = totalCount/80.0;
    int percent = (count+1)/factor;
    int number = percent/5;
    int remainder = percent%5;
    if(number > 0)
    {
      for(int j = 0; j < number; j++)
      {
        lcd.setCursor(j,lineToPrintOn);
       lcd.write(5);
      }
    }
       lcd.setCursor(number,lineToPrintOn);
       lcd.write(remainder); 
     if(number < 16)
    {
      for(int j = number+1; j <= 16; j++)
      {
        lcd.setCursor(j,lineToPrintOn);
       lcd.write(0);
      }
    }  
 }

 // Lights On functions
void trellisStartup(){
  //activate all keys and set callbacks
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, blink);
  }
}

void trellisShutDown(){
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING, false);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING, false);
    trellis.unregisterCallback(i);
  }
}

void trellisStartupAnimation(){
  //do a little animation to show we're on
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(50);
  }
}

void checkFinished(){
  trellisSum = 0;
  for(int i = 0; i < trellisArraySize; i++){
    trellisSum += trellisArray[i];
  }
  Serial.print("Sum of trellis array: ");
  Serial.println(trellisSum);
  if(trellisSum == 16){
    Serial.println("You're finished");
    finishSequence();
  }
}

void finishSequence(){
  trellisSum = 0;
  emptyTrellisArray();
  //rainbowPulse();
  //rainbowChaseBorder();
  fireworks();
  for(int k = 0; k < trellis.pixels.numPixels(); k++){
    trellis.pixels.setPixelColor(k, noLight);
  }
  trellis.pixels.show();
  trellisShutDown();
  LO_finished = true;
}

void emptyTrellisArray(){
  for(int i = 0; i < trellisArraySize; i++){
    trellisArray[i] = 0;
  }
}

void cheatCode(int buttonPressed){
  if(buttonPressed == 0){
    cheatCodeNr++;
    Serial.print("cheatCodeNr: ");
    Serial.println(cheatCodeNr);
    if(cheatCodeNr == 5){
      Serial.println("Cheat code used");
      blinkRed();
      trellisShutDown();
    }
  }
  else{
    cheatCodeNr = 0;
  }
}

void LO_buttonCheck(int buttonPressed){
  for(int i = 0; i < regularButtonsArraySize; i++){
    if(buttonPressed == regularButtonsArray[i]){
      //Serial.println("The button pressed is 0, 1, 2, 5, 6, 9, 10, 13, 14, or 15");
      regularAdjButtons(buttonPressed);
    }
  }
  for(int i = 0; i < leftButtonsArraySize; i++){
    if(buttonPressed == leftButtonsArray[i]){
      //Serial.println("The button pressed is 3, 7, or 11");
      leftAdjButtons(buttonPressed);
    }
  }
  for(int i = 0; i < rightButtonsArraySize; i++){
    if(buttonPressed == rightButtonsArray[i]){
      //Serial.println("The button pressed is 4, 8, or 12");
      rightAdjButtons(buttonPressed);
    }
  }
}

void regularAdjButtons(int buttonPressed){
  buttonAdjUp = buttonPressed-4;
  buttonAdjRight = buttonPressed+1;
  buttonAdjDown = buttonPressed+4;
  buttonAdjLeft = buttonPressed-1;

  buttonPressedColor = trellis.pixels.getPixelColor(buttonPressed);
  buttonAdjUpColor = trellis.pixels.getPixelColor(buttonAdjUp);
  buttonAdjRightColor = trellis.pixels.getPixelColor(buttonAdjRight);
  buttonAdjDownColor = trellis.pixels.getPixelColor(buttonAdjDown);
  buttonAdjLeftColor = trellis.pixels.getPixelColor(buttonAdjLeft);

  // Checking and Flipping buttonPressed
  if((unsigned long)buttonPressedColor == red){
    trellis.pixels.setPixelColor(buttonPressed, noLight);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 0; 
    }
  }
  else if((unsigned long)buttonPressedColor == noLight){
    trellis.pixels.setPixelColor(buttonPressed, red);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 1; 
    }
  }

  // Checking and flipping the button above the button pressed
  if((unsigned long)buttonAdjUpColor == red){
    trellis.pixels.setPixelColor(buttonAdjUp, noLight);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 0;
    }
  }
  else if((unsigned long)buttonAdjUpColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjUp, red);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 1;
    }
  }

  // Checking and flipping the button to the right of the button pressed
  if((unsigned long)buttonAdjRightColor == red){
    trellis.pixels.setPixelColor(buttonAdjRight, noLight);
    if((buttonAdjRight - 16)*(buttonAdjRight - 0) <= 0){
      trellisArray[buttonAdjRight] = 0;
    }
  }
  else if((unsigned long)buttonAdjRightColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjRight, red);
    if((buttonAdjRight - 16)*(buttonAdjRight - 0) <= 0){
      trellisArray[buttonAdjRight] = 1;
    }
  }

  // Checking and flipping the color bellow the button pressed
  if((unsigned long)buttonAdjDownColor == red){
    trellis.pixels.setPixelColor(buttonAdjDown, noLight);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 0;
    }
  }
  else if((unsigned long)buttonAdjDownColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjDown, red);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 1;
    }
  }

  // Checking and flipping the color to the left of the button pressed
  if((unsigned long)buttonAdjLeftColor == red){
    trellis.pixels.setPixelColor(buttonAdjLeft, noLight);
    if((buttonAdjLeft - 16)*(buttonAdjLeft - 0) <= 0){
      trellisArray[buttonAdjLeft] = 0;
    }
  }
  else if((unsigned long)buttonAdjLeftColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjLeft, red);
    if((buttonAdjLeft - 16)*(buttonAdjLeft - 0) <= 0){
      trellisArray[buttonAdjLeft] = 1;
    }
  }
}

void leftAdjButtons(int buttonPressed){
  buttonAdjUp = buttonPressed-4;
  buttonAdjDown = buttonPressed+4;
  buttonAdjLeft = buttonPressed-1;

  buttonPressedColor = trellis.pixels.getPixelColor(buttonPressed);
  buttonAdjUpColor = trellis.pixels.getPixelColor(buttonAdjUp);
  buttonAdjDownColor = trellis.pixels.getPixelColor(buttonAdjDown);
  buttonAdjLeftColor = trellis.pixels.getPixelColor(buttonAdjLeft);

  // Checking and Flipping buttonPressed
  if((unsigned long)buttonPressedColor == red){
    trellis.pixels.setPixelColor(buttonPressed, noLight);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 0; 
    }
  }
  else if((unsigned long)buttonPressedColor == noLight){
    trellis.pixels.setPixelColor(buttonPressed, red);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 1; 
    }
  }

  // Checking and flipping the button above the button pressed
  if((unsigned long)buttonAdjUpColor == red){
    trellis.pixels.setPixelColor(buttonAdjUp, noLight);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 0;
    }
  }
  else if((unsigned long)buttonAdjUpColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjUp, red);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 1;
    }
  }

  // Checking and flipping the color bellow the button pressed
  if((unsigned long)buttonAdjDownColor == red){
    trellis.pixels.setPixelColor(buttonAdjDown, noLight);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 0;
    }
  }
  else if((unsigned long)buttonAdjDownColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjDown, red);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 1;
    }
  }

  // Checking and flipping the color to the left of the button pressed
  if((unsigned long)buttonAdjLeftColor == red){
    trellis.pixels.setPixelColor(buttonAdjLeft, noLight);
    if((buttonAdjLeft - 16)*(buttonAdjLeft - 0) <= 0){
      trellisArray[buttonAdjLeft] = 0;
    }
  }
  else if((unsigned long)buttonAdjLeftColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjLeft, red);
    if((buttonAdjLeft - 16)*(buttonAdjLeft - 0) <= 0){
      trellisArray[buttonAdjLeft] = 1;
    }
  }
}

void rightAdjButtons(int buttonPressed){
  buttonAdjUp = buttonPressed-4;
  buttonAdjRight = buttonPressed+1;
  buttonAdjDown = buttonPressed+4;

  buttonPressedColor = trellis.pixels.getPixelColor(buttonPressed);
  buttonAdjUpColor = trellis.pixels.getPixelColor(buttonAdjUp);
  buttonAdjRightColor = trellis.pixels.getPixelColor(buttonAdjRight);
  buttonAdjDownColor = trellis.pixels.getPixelColor(buttonAdjDown);

  // Checking and Flipping buttonPressed
  if((unsigned long)buttonPressedColor == red){
    trellis.pixels.setPixelColor(buttonPressed, noLight);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 0; 
    }
  }
  else if((unsigned long)buttonPressedColor == noLight){
    trellis.pixels.setPixelColor(buttonPressed, red);
    if((buttonPressed - 16)*(buttonPressed - 0) <= 0){
     trellisArray[buttonPressed] = 1; 
    }
  }

  // Checking and flipping the button above the button pressed
  if((unsigned long)buttonAdjUpColor == red){
    trellis.pixels.setPixelColor(buttonAdjUp, noLight);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 0;
    }
  }
  else if((unsigned long)buttonAdjUpColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjUp, red);
    if((buttonAdjUp - 16)*(buttonAdjUp - 0) <= 0){
      trellisArray[buttonAdjUp] = 1;
    }
  }

  // Checking and flipping the button to the right of the button pressed
  if((unsigned long)buttonAdjRightColor == red){
    trellis.pixels.setPixelColor(buttonAdjRight, noLight);
    if((buttonAdjRight - 16)*(buttonAdjRight - 0) <= 0){
      trellisArray[buttonAdjRight] = 0;
    }
  }
  else if((unsigned long)buttonAdjRightColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjRight, red);
    if((buttonAdjRight - 16)*(buttonAdjRight - 0) <= 0){
      trellisArray[buttonAdjRight] = 1;
    }
  }

  // Checking and flipping the color bellow the button pressed
  if((unsigned long)buttonAdjDownColor == red){
    trellis.pixels.setPixelColor(buttonAdjDown, noLight);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 0;
    }
  }
  else if((unsigned long)buttonAdjDownColor == noLight){
    trellis.pixels.setPixelColor(buttonAdjDown, red);
    if((buttonAdjDown - 16)*(buttonAdjDown - 0) <= 0){
      trellisArray[buttonAdjDown] = 1;
    }
  }
}

void rainbowChaseBorder(){
  for(int j = 0; j < 256; j=j+20){
    for(int i = 0; i < animateRBArraySize; i++){
      trellis.pixels.setPixelColor(animateRBArray[i], Wheel( (i+j) % 255));
      trellis.pixels.show();
      delay(100);
    }
  }
  for(int k = 0; k < trellis.pixels.numPixels(); k++){
    trellis.pixels.setPixelColor(k, noLight);
  }
  trellis.pixels.show();
}

void fireworks(){
  for(int i = 0; i < fireworksColorsSize; i++){
    trellis.pixels.setPixelColor(5, fireworksColors[i]);
    trellis.pixels.setPixelColor(6, fireworksColors[i]);
    trellis.pixels.setPixelColor(9, fireworksColors[i]);
    trellis.pixels.setPixelColor(10, fireworksColors[i]);
    trellis.pixels.show();
    delay(200);
    trellis.pixels.setPixelColor(0, fireworksColors[i]);
    trellis.pixels.setPixelColor(1, fireworksColors[i]);
    trellis.pixels.setPixelColor(2, fireworksColors[i]);
    trellis.pixels.setPixelColor(3, fireworksColors[i]);
    trellis.pixels.setPixelColor(4, fireworksColors[i]);
    trellis.pixels.setPixelColor(7, fireworksColors[i]);
    trellis.pixels.setPixelColor(8, fireworksColors[i]);
    trellis.pixels.setPixelColor(11, fireworksColors[i]);
    trellis.pixels.setPixelColor(12, fireworksColors[i]);
    trellis.pixels.setPixelColor(13, fireworksColors[i]);
    trellis.pixels.setPixelColor(14, fireworksColors[i]);
    trellis.pixels.setPixelColor(15, fireworksColors[i]);
    trellis.pixels.show();
    delay(200);
  }
}

void rainbowPulse() {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 1; q++) {
      for (uint16_t i=0; i < trellis.pixels.numPixels(); i=i+1) {
        trellis.pixels.setPixelColor(i+q, Wheel( (i+j) % 255));
      }
      trellis.pixels.show();

      delay(100);

      for (uint16_t i=0; i < trellis.pixels.numPixels(); i=i+1) {
        trellis.pixels.setPixelColor(i+q, 0);
      }
    }
  }
  delay(500);
  for(int k = 0; k < trellis.pixels.numPixels(); k++){
    trellis.pixels.setPixelColor(k, noLight);
  }
  trellis.pixels.show();
}

void blinkRed(){
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < trellis.pixels.numPixels(); j++){
      trellis.pixels.setPixelColor(j, red);
    }
    trellis.pixels.show();
    delay(100);
    for(int k = 0; k < trellis.pixels.numPixels(); k++){
      trellis.pixels.setPixelColor(k, noLight);
    }
    trellis.pixels.show();
    delay(100);
  }
}