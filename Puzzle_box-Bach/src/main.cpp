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

//I2C slaves defines
#define slaveAdr1 0x01
#define slaveAdr2 0x02

//Define variable to send to slaves
int RtN = 1;
int FtS = 2;
int CtC = 3;
int WDT_reset = 9;

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
const int toggle_switch = 9;

//Variables for LED strip
#define LEDpin 6
#define numPixels 92

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

//Set up LED strip, with # of pixels, pin #, and type
Adafruit_NeoPixel strip(numPixels, LEDpin, NEO_GRB + NEO_KHZ800);

// Menu variable that always starts at 1
int mainMenu = 1;

//These 5 arrays paint the bars that go across the screen.  
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

boolean moduleFinished = false;

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

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  updateMainMenu();

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  startUpLights();

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
}

void startUpLights(){
  for(int i = 0; i < numPixels; i++){
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(100);
  }
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
void debounceMenuButtonEsc(){
  int menuButtonEsc_reading = digitalRead(menuButtonEsc);
  
  if (menuButtonEsc_reading != menuButtonEsc_lastState){
    lastDebounceTimeMenu = millis();
  }

  if ((millis() - lastDebounceTimeMenu) > debounceDelay){
    if (menuButtonEsc_reading != menuButtonEsc_state){
      menuButtonEsc_state = menuButtonEsc_reading;
      if (menuButtonEsc_state == LOW){
        Serial.println("Escape button has been pressed");
        Serial.println("Esc button pressed: short press");
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
        Serial.println("The bomb has been disarmed");
      }
      if (toggleSwitch_state == HIGH){
        Serial.println("Bomb has been armed");
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
  delay(1000);
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
}

void LO_func(){
  moduleChosen = true;
  Serial.println("LO function running");
  delay(1000);
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
  requestSlave2();

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
  if(masterReceiveSlave2 == 0){
    noSignal_slave2(); 
  }
  else if(masterReceiveSlave2 == 2){
    Serial.println("FtS module done!");
  }
  else if(masterReceiveSlave2 == 3){
    Serial.println("CtC module done!");
  }
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
    lcd.print(" %"); 
    lcd.print("   ");
    updateProgressBar(i, 100, 1);
    delay(100);
  }
}

// Not own code, taken from: https://www.instructables.com/Simple-Progress-Bar-for-Arduino-and-LCD/
void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn){
  double factor = totalCount/80.0;          
  int percent = (count+1)/factor;
  int number = percent/5;
  int remainder = percent%5;
  if(number > 0)
  {
    lcd.setCursor(number-1,lineToPrintOn);
    lcd.write(5);
  }
  lcd.setCursor(number,lineToPrintOn);
  lcd.write(remainder);
}