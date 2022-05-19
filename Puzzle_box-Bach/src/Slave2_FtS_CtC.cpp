
#include <Wire.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_LEDBackpack.h>

// Watchdog interrupt Reset for slave 2 MCU
#define WDT_CTRL *(uint8_t*) (0x40001000+0x00)

// Module connection pins 
#define CLK A5
#define DIO A6 
#define LED_COUNT_Numbers  4                                                                     
#define LED_COUNT_Progress  5                                                                    // How many NeoPixels are attached to the Arduino?
#define LED_COUNT_Numbers  4                                                                     // How many NeoPixels are attached to the Arduino?
#define BRIGHTNESS 50                                                                            // NeoPixel brightness, 0 (min) to 255 (max)
#define RandomizeNumberPin A0                                                                    // For random(); function in UnlockingSecuence function()

 // Data in pins for CtC
const int           LedStripNumberzPin      = A3;                                                // Data in Pin for the Led Strip indicator
const int           RotaryNC_Pin            = 5;                                                 // Data in Pin r
const int           RotarySW1_Pin           = 6;                                                 // Data in Pin 
const int           RotarySW2_Pin           = 7;                                                 // Data in Pin 

// Data in pins for FtS
const int           BlueButtonPin           = 0;                                                 // Blue ToggleSwitch pin
const int           RedButtonPin            = 1;                                                 // Red ToggleSwitch pin
const int           GreenButtonPin          = 2;                                                 // Green ToggleSwitch pin
const int           OrangeButtonPin         = 3;                                                 // Orange ToggleSwitch pin
const int           WhiteButtonPin          = 4;                                                 // White ToggleSwitch pin
const int           LedStripProgressPin     = A1;                                                // Data in Pin for the Led Strip indicator
const int           LedStripNumbersPin      = A2;                                                // Data in Pin for the Led Strip indicator
const int           StartButtonPin          = A4;                                                // Data in pin Start button

//Variables for CtC module
int                 RotarySelect            = HIGH;                                              // variable to store the Rotary switch status
int                 RotaryRight             = HIGH;                                              // variable to store the Rotary switch status
int                 RotaryLeft              = HIGH;                                              // variable to store the Rotary switch status
int                 SelectDigit             = 0;                                                 // Defines what digit currently being incremented/decremented
int                 Attempts                = 13;                                                // Defines how many attempts are allowed for the Crack the Code module
int                 SelectStateNow          = 0;                                                 // Variable to store the current state of the Rotary switch
int                 RightStateNow           = 0;                                                 // Variable to store the current state of the Rotary switch
int                 LeftStateNow            = 0;                                                 // Variable to store the current state of the Rotary switch
int                 SelectPreviousState     = 0;                                                 // Variable to store the Previous state of the Rotary switch
int                 RightPreviousState      = 0;                                                 // Variable to store the Previous state of the Rotary switch
int                 LeftPreviousState       = 0;                                                 // Variable to store the Previous state of the Rotary switch
int                 Password                = 0;                                                 // Defines what password the program checks for in Crack the Code. 
int                 MyRotaryRegister[3]     = {0, 0, 0};                                         // Array to store the inputs from the outputs of the Rotary Switch
int                 RotaryPrevious[3]       = {0, 0, 0};                                         // Array to store the previous input from the Rotary switch
int                 Action[3]               = {0, 0, 0};                                         // Array to store what action to take based on inputs from the Rotary switch

// More variables for CtC
const unsigned int  Password1[4]            = {3, 7, 2, 5};                                      // The first password for Crack the Code module
const unsigned int  Password2[4]            = {7, 0, 3, 4};                                      // The second password for Crack the Code module 
unsigned int        Digit[4]                = {0, 0, 0, 0};                                      // This array stores the inputs in Crack the Code 
unsigned long       CtC_DebounceDelay       = 20;                                                // Debounce delay for toggle switches
unsigned long       CtC_DebounceTimer       = 0;                                                 // Stores the current value of millis()   
uint8_t             data[4]                 = {0xff, 0xff, 0xff, 0xff };                         // The array that stores the variables on the 4 digit display for Crack the Code module                                                              
const uint8_t       SEG_SWIRL1[]            = {SEG_A, SEG_A, 0, 0};                              // Lights up the two leftmost segments on the top of the display
const uint8_t       SEG_SWIRL2[]            = {0, 0, SEG_A, SEG_A};                              // Lights up to segments on the 4 digit with 7 segments per digit display
const uint8_t       SEG_SWIRL3[]            = {0 ,0 ,0 ,SEG_B | SEG_C};                          // Lights up to segments on the 4 digit with 7 segments per digit display
const uint8_t       SEG_SWIRL4[]            = {0, 0, SEG_D, SEG_D};                              // Lights up to segments on the 4 digit with 7 segments per digit display
const uint8_t       SEG_SWIRL5[]            = {SEG_D, SEG_D, 0, 0};                              // Lights up to segments on the 4 digit with 7 segments per digit display
const uint8_t       SEG_SWIRL6[]            = {SEG_E | SEG_F, 0, 0, 0};                          // Lights up to segments on the 4 digit with 7 segments per digit display
const uint8_t       SEG_DONE[]              = {SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, SEG_C | SEG_E | SEG_G, SEG_A | SEG_D | SEG_E | SEG_F | SEG_G};
const uint8_t       SEG_PASS[]              = {SEG_A | SEG_B | SEG_E | SEG_F | SEG_G , SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G , SEG_A | SEG_C | SEG_D | SEG_G | SEG_F , SEG_A | SEG_C | SEG_D | SEG_G | SEG_F};
const uint8_t       SEG_FAIL[]              = {SEG_A | SEG_F | SEG_E | SEG_G , SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G , SEG_E | SEG_F, SEG_F | SEG_E | SEG_D};
const uint8_t       SEG_BLINK[]             = {SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G, SEG_A | SEG_D | SEG_G};
const uint8_t       SEG_BOOM[]              = {SEG_E | SEG_G, SEG_E | SEG_F, SEG_E | SEG_F | SEG_A | SEG_B | SEG_G, 0};
TM1637Display       display                   (CLK, DIO);                                                       
Adafruit_NeoPixel   stripNumberz              (LED_COUNT_Numbers, LedStripNumberzPin, NEO_RGB + NEO_KHZ800); 

// Variables for FtS
int                 Start                   = LOW; 
int                 GameStarting            = 1;
int                 MyRegister[5]           = {0, 0, 0, 0, 0};                                   // Saves the states of toggle switches in an array
int                 Progress[5]             = {0, 0, 0, 0, 0};                                   // Saves the progress in an array
int                 SwitchPrevious[5]       = {0, 0, 0, 0, 0};                                   // Savess the previous state of the toggle switches for the Switch-Case
int                 UnlockingSequence[5]    = {0, 0, 0, 0, 0};                                   // {8, 3, 5, 9, 7}; This array stores the numbers on the timer where individual switches can be toggled to get a "pass", UnlockingSequence[0] is for the blue toggle switch etc..
int                 SequenceCounter         = 0;                                                 // Update UnlockingSequence every 15 second
int                 PreviousNumber          = 0;                                                 // For NumbersIndicator() so that the same inary number is not displayed twice in a row


// variables for FtS module:
int                 BlueButtonState         = LOW;                                               // variable for reading the toggle switch status
int                 RedButtonState          = LOW;                                               // variable for reading the toggle switch status
int                 GreenButtonState        = LOW;                                               // variable for reading the toggle switch status
int                 OrangeButtonState       = LOW;                                               // variable for reading the toggle switch status
int                 WhiteButtonState        = LOW;                                               // variable for reading the toggle switch status

int                 BlueStateNow            = 0;
int                 RedStateNow             = 0;
int                 GreenStateNow           = 0;
int                 OrangeStateNow          = 0;
int                 WhiteStateNow           = 0;
int                 S                       = 0;                                                // S variable that is set to 1 to run the code that resets the MCU

//boolean buttonState = LOW;
int                 BluePreviousState       = LOW;                                              // Saves the previous state of the toggle switch 
int                 RedPreviousState        = LOW;                                              // Saves the previous state of the toggle switch
int                 GreenPreviousState      = LOW;                                              // Saves the previous state of the toggle switch
int                 OrangePreviousState     = LOW;                                              // Saves the previous state of the toggle switch
int                 WhitePreviousState      = LOW;                                              // Saves the previous state of the toggle switch 


unsigned long       FtS_DebounceDelay       = 50;                                                         // Debounce delay for toggle switches
unsigned long       FtS_DebounceTimer       = 0;                                                          // Stores the current value of millis()
unsigned long       LoopDelay               = 1000;                                                       // ticks down starting at 1000ms rate (normal speed)
unsigned long       LoopTimer               = 0;                                                          // Stores the current value of millis()
unsigned long       LoopSubtract            = 0;
unsigned long       PassDebounceTimer       = 0;                                                          // Stores the current value of millis()        
unsigned long       PassDebounceDelay       = 50;                                                         // Debounce delay after a successful toggle 
unsigned long       StartPressed            = 0;                
unsigned long       StartSignal             = 5000;
unsigned long       ProgressBar[5]          = {0, 0, 0, 0, 0};                                            // Display the current progress on "progress inticator strip"
unsigned long       BRGOW[5]                = {0x0000FF, 0x00FF00, 0xFF0000, 0x40FF00, 0xFFFFFF};         // Blue, Red, Green, Orange, White GRB HEX
Adafruit_7segment   matrix                  = Adafruit_7segment();
boolean             drawDots                = true;
uint16_t            counterD                = 0;





Adafruit_NeoPixel stripProgress(LED_COUNT_Progress, LedStripProgressPin, NEO_RGB + NEO_KHZ800);           // Declare our NeoPixel strip object
Adafruit_NeoPixel stripNumbers(LED_COUNT_Numbers, LedStripNumbersPin, NEO_RGB + NEO_KHZ800);              // Declare our NeoPixel strip object
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

//Define variable to receive
byte receiveFromMaster = 0;

//Define variable to send
byte sendToMaster = 0;

// Variable determining what module is running 
int U = 0;

bool CtC_complete = false;
bool FtS_complete = false;

void ProgressCheck();
void LedIndicator();
void DisplaySwirl();
void DisplayBlink(); 
void EnterPassword();
void DebounceSelect();
void DebounceRight();
void DebounceLeft();
void StartButton();
void NumbersIndicator();
void ProgressIndicator();
void UpdateSequence();
void GameProgress();
void AccelerateTimer();
void DebounceBlue();
void DebounceRed();
void DebounceGreen();
void DebounceOrange();
void DebounceWhite();
void receiveEvent(int);
void requestEvent();
void playCtC();
void playFtS();
void SelectDigitz();
void ChooseGame();
void Switch();





void setup()
{
  Serial.begin(9600);

  Wire.begin(0x79);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  delay(1000);

 //For CtC !!
 pinMode(RotaryNC_Pin, INPUT_PULLUP);        // Initialize Toggle switch pins as NC
 pinMode(RotarySW1_Pin, INPUT_PULLUP);
 pinMode(RotarySW2_Pin, INPUT_PULLUP);
 pinMode(LedStripNumberzPin, OUTPUT);
 stripNumberz.begin();                        // INITIALIZE NeoPixel strip object (REQUIRED)
 stripNumberz.clear();
 stripNumberz.show();                         // Turn OFF all pixels ASAP
 stripNumberz.setBrightness(BRIGHTNESS);
 display.setBrightness(BRIGHTNESS);
 display.clear();

 //For FtS !!
 pinMode(BlueButtonPin, INPUT_PULLDOWN);        // Initialize Toggle switch pins as NO
 pinMode(RedButtonPin, INPUT_PULLDOWN);
 pinMode(GreenButtonPin, INPUT_PULLDOWN);
 pinMode(OrangeButtonPin, INPUT_PULLDOWN);
 pinMode(WhiteButtonPin, INPUT_PULLDOWN);
 pinMode(StartButtonPin, INPUT_PULLUP);       // Initialize Start button        A4 -> Allow_Start NC starts LOW (INPUT_PULLUP, when INPUT goes HIGH from PushButton, Allow_Start is set to HIGH)
 pinMode(LedStripProgressPin, OUTPUT);        // Initialize LED strip pins as output
 pinMode(LedStripNumbersPin, OUTPUT);
 randomSeed(analogRead(RandomizeNumberPin));
 stripProgress.begin();                       // INITIALIZE NeoPixel strip object (REQUIRED)
 stripProgress.clear();                      //ADDED CLEARS AFTER TESTING !!
 stripNumbers.begin();                       //ADDED .clear() after testing
 stripNumbers.clear();                      // INITIALIZE NeoPixel strip object (REQUIRED)
 stripProgress.show();                        // Turn OFF all pixels ASAP
 stripNumbers.show();                         // Turn OFF all pixels ASAP
 stripProgress.setBrightness(BRIGHTNESS); 
 stripNumbers.setBrightness(BRIGHTNESS);
 delay(1000);
}


void loop()
{
  if (U == 4)
  {
   U = 0;
   Serial.println("Test");
  }
ChooseGame();
}

void ChooseGame()
{
  while(U != 2 || U != 3)
  {
        if (U == 2)
        {
          playFtS();
          U = 0;
        }

        if (U == 3)
        {
          playCtC();  
          U = 0;
        }
  }

}

void ProgressCheck()
{
if ((Progress[0] == 1 && Progress[1] == 1 && Progress[2] == 1 && Progress[3] == 1 && Progress[4] == 1) && 
    (MyRegister[0] == 1 && MyRegister[1] == 1 && MyRegister[2] == 1 && MyRegister[3] == 1 && MyRegister[4] == 1))
  {
    U = 0;
    matrix.clear();
    matrix.writeDisplay();
    Wire.endTransmission(0x71); 
    Wire.begin(0x79);
    delay(2000);
    sendToMaster = 2;
    FtS_complete = true;
    delay(5000);
  }
}

 void LedIndicator()
 {
   if ((Digit[0] + Digit[1] + Digit[2] + Digit[3]) == 0) {stripNumberz.clear(); stripNumberz.show();}
   if ((Digit[0] + Digit[1] + Digit[2] + Digit[3]) != 0)
   {
    for (int i = 0; i < 4; i++)
     { 
       switch(Digit[i])
        {
          case 0:
            if (Password == 0){stripNumberz.setPixelColor(i, 0x00FF00); break;}              // Red
            if (Password == 1 && i == 1){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 1 && i != 1){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 1:
            stripNumberz.setPixelColor(i, 0x00FF00);                                         // Red
            break;

          case 2:
            if (Password == 1){stripNumberz.setPixelColor(i, 0x00FF00); break;}              // Red
            if (Password == 0 && i == 2){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 0 && i != 2){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 3:
            if (Password == 0 && i == 0){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 0 && i != 0){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange            
            if (Password == 1 && i == 2){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 1 && i != 2){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 4:
            if (Password == 0){stripNumberz.setPixelColor(i, 0x00FF00); break;}              // Red
            if (Password == 1 && i == 3){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 1 && i != 3){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 5:
            if (Password == 1){stripNumberz.setPixelColor(i, 0x00FF00); break;}              // Red
            if (Password == 0 && i == 3){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 0 && i != 3){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 6:
            stripNumberz.setPixelColor(i, 0x00FF00);                                         // Red
            break;

          case 7:
            if (Password == 0 && i == 1){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 0 && i != 1){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange            
            if (Password == 1 && i == 0){stripNumberz.setPixelColor(i, 0xFF0000); break;}    // Green
            if (Password == 1 && i != 0){stripNumberz.setPixelColor(i, 0x40FF00); break;}    // Orange

          case 8:
            stripNumberz.setPixelColor(i, 0x00FF00);                                         // Red
            break;

          case 9:
            stripNumberz.setPixelColor(i, 0x00FF00);                                         // Red
            break;
          
          default:
            stripNumberz.setPixelColor(i, 0x00FF00);                                         // Red 
            break;
        }
      }    
    stripNumberz.show(); 
    delay(1000); 
    memset(Digit, 0, sizeof(Digit));
    memset(data, 0, sizeof(data)); 
  }
}

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
        display.clear();
        SelectDigit = 0;
        for (int h = 0; h < 4; h++)
        {stripNumberz.clear(); stripNumberz.setPixelColor(h, 0xFF0000); stripNumberz.show(); delay(500);} 
        stripNumberz.clear();
        stripNumberz.setPixelColor(0, 0xFF0000);
        stripNumberz.setPixelColor(1, 0xFF0000);
        stripNumberz.setPixelColor(2, 0xFF0000);
        stripNumberz.setPixelColor(3, 0xFF0000);
        stripNumberz.show();
        delay(1000);
        memset(Digit, 0, sizeof(Digit));
        //stripNumberz.clear();
      }
  if (Password == 1 && (Digit[0] == Password2[0] && Digit[1] == Password2[1] && Digit[2] == Password2[2] && Digit[3] == Password2[3]))
      {
        Serial.println("Bomb has been defused");
        DisplaySwirl();
        display.setSegments(SEG_PASS);
        delay(2000);
        DisplayBlink();
        stripNumberz.clear();
        display.setSegments(SEG_DONE);
        U = 0;
        sendToMaster = 3;
        CtC_complete = true;
        delay(3000);
        stripNumberz.show();
      }
}

void DebounceSelect()
{
  SelectStateNow = digitalRead(RotaryNC_Pin);
  if(SelectStateNow != SelectPreviousState)
  {
    CtC_DebounceTimer = millis();
  }
  if ((millis() - CtC_DebounceTimer) > CtC_DebounceDelay)
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
    CtC_DebounceTimer = millis();
  }
  if ((millis() - CtC_DebounceTimer) > CtC_DebounceDelay)
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
    CtC_DebounceTimer = millis();
  }
  if ((millis() - CtC_DebounceTimer) > CtC_DebounceDelay)
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



void StartButton()
{
  if (digitalRead(StartButtonPin) == HIGH) 
  {
    StartPressed = millis();
   while (digitalRead(StartButtonPin) == HIGH)
    {
     if ((millis() - StartPressed) > StartSignal)
       { Serial.println("starting..");
         Start = HIGH;
         Wire.end();
         matrix.begin(0x71);
         matrix.clear();
         matrix.writeDisplay();
       }
    }
  }
}

void NumbersIndicator(){
 int x = counterD;
 if (x == 5) {x = 0;}
 if (x == 6) {x = 1;}
 if (x == 7) {x = 2;}
 if (x == 8) {x = 3;}
 if (x == 9) {x = 4;}
 if (x != PreviousNumber)   // Første LED fra Din er MSB i den binære koden som vises
{
    switch(UnlockingSequence[x]){
     case 0:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, 0);
     stripNumbers.setPixelColor(2, 0);stripNumbers.setPixelColor(3, 0);
     break;
     case 1:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, 0); 
     stripNumbers.setPixelColor(2, 0); stripNumbers.setPixelColor(3, BRGOW[x]);
     break;
     case 2:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, 0); 
     stripNumbers.setPixelColor(2, BRGOW[x]); stripNumbers.setPixelColor(3, 0);
     break;
     case 3:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, 0); 
     stripNumbers.setPixelColor(2, BRGOW[x]); stripNumbers.setPixelColor(3, BRGOW[x]);
     break;
     case 4:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, BRGOW[x]); 
     stripNumbers.setPixelColor(2, 0); stripNumbers.setPixelColor(3, 0);
     break;
     case 5:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, BRGOW[x]); 
     stripNumbers.setPixelColor(2, 0);stripNumbers.setPixelColor(3, BRGOW[x]);
     break;
     case 6:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, BRGOW[x]); 
     stripNumbers.setPixelColor(2, BRGOW[x]); stripNumbers.setPixelColor(3, 0);
     break;
     case 7:
     stripNumbers.setPixelColor(0, 0); stripNumbers.setPixelColor(1, BRGOW[x]); 
     stripNumbers.setPixelColor(2, BRGOW[x]); stripNumbers.setPixelColor(3, BRGOW[x]);
     break;
     case 8:
     stripNumbers.setPixelColor(0, BRGOW[x]); stripNumbers.setPixelColor(1, 0); 
     stripNumbers.setPixelColor(2, 0); stripNumbers.setPixelColor(3, 0);
     break;
     case 9:
     stripNumbers.setPixelColor(0, BRGOW[x]); stripNumbers.setPixelColor(1, 0); 
     stripNumbers.setPixelColor(2, 0); stripNumbers.setPixelColor(3, BRGOW[x]);
     break;
     default:
     break;}
    PreviousNumber = x;
    stripNumbers.show();
  } 
} 

void ProgressIndicator()
{
  for(int p = 0; p < 5; p++)
   {
     if((Progress[p] == MyRegister[p]) && MyRegister[p] == 1)
       {
         if(p == 0) {ProgressBar[p] = 0x0000FF;}    // BLUE
         if(p == 1) {ProgressBar[p] = 0x00FF00;}    // RED
         if(p == 2) {ProgressBar[p] = 0xFF0000;}    // GREEN
         if(p == 3) {ProgressBar[p] = 0x30FF00;}    // ORANGE
         if(p == 4) {ProgressBar[p] = 0xFFFFFF;}    // WHITE
         stripProgress.setPixelColor(p, ProgressBar[p]); 
        }
     if ((MyRegister[p] == 0) || (Progress[p] == 0))
        {
          stripProgress.setPixelColor(p, 0);
        } 
    }
    stripProgress.show();
}

void UpdateSequence()
{
  
  if ((SequenceCounter > 60) || GameStarting == 1)
  {
   for(int i = 0; i < 5; i++)
   {
     UnlockingSequence[i] = random(10);
   }
    GameStarting = 0;
    SequenceCounter = 0;
    Serial.println(" ");
    for (int s = 0; s < 5; s++)
    {
      Serial.print(UnlockingSequence[s]);
    }
    Serial.println(" <-- Unlocking sequence UPDATED");
  }
}

void GameProgress()
{
  Serial.println("Pass ");
}

void AccelerateTimer()
{
  Serial.println("Fail ");
  if (LoopDelay > 200)
  {
    MyRegister[0] = 0; 
    LoopDelay = (LoopDelay - 50); // Timer ticks down at a faster rate after failure (wrong input)
    Serial.print("LoopDelay: ");
    Serial.println(LoopDelay, DEC);
  }
  if (LoopDelay <= 200)
  {
    LoopDelay = (1000 - LoopSubtract);
    LoopSubtract = (LoopSubtract + 200);
  }
  delay(50);
}

void DebounceBlue()
{
  BlueStateNow = digitalRead(BlueButtonPin);
  if(BlueStateNow != BluePreviousState)
  {
    FtS_DebounceTimer = millis();
  }
  if ((millis() - FtS_DebounceTimer) > FtS_DebounceDelay)
  {
    if(BlueStateNow != BlueButtonState)
    {
      BlueButtonState = BlueStateNow;
      if (BlueButtonState == LOW)
        {
          MyRegister[0] = 1;   
        }
      if (BlueButtonState == HIGH) 
        {
          MyRegister[0] = 0;   
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
    FtS_DebounceTimer = millis();
  }
  if ((millis() - FtS_DebounceTimer) > FtS_DebounceDelay)
  {
    if(RedStateNow != RedButtonState)
    {
      RedButtonState = RedStateNow;
      if (RedButtonState == LOW)
        {
          MyRegister[1] = 1;   // OFF
        }
      if (RedButtonState == HIGH) 
        {
          MyRegister[1] = 0;   // ON
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
    FtS_DebounceTimer = millis();
  }
  if ((millis() - FtS_DebounceTimer) > FtS_DebounceDelay)
  {
    if(GreenStateNow != GreenButtonState)
    {
      GreenButtonState = GreenStateNow;
      if (GreenButtonState == LOW)
        {
          MyRegister[2] = 1;   // OFF
        }
      if (GreenButtonState == HIGH) 
        {
          MyRegister[2] = 0;   // ON
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
    FtS_DebounceTimer = millis();
  }
  if ((millis() - FtS_DebounceTimer) > FtS_DebounceDelay)
  {
    if(OrangeStateNow != OrangeButtonState)
    {
      OrangeButtonState = OrangeStateNow;
      if (OrangeButtonState == LOW)
        {
          MyRegister[3] = 1;  //OFF
        }
      if (OrangeButtonState == HIGH) 
        {
          MyRegister[3] = 0;  //ON
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
    FtS_DebounceTimer = millis();
  }
  if ((millis() - FtS_DebounceTimer) > FtS_DebounceDelay)
  {
    if(WhiteStateNow != WhiteButtonState)
    {
      WhiteButtonState = WhiteStateNow;
      if (WhiteButtonState == LOW)
        {
          MyRegister[4] = 1;   //Switchen er NC, og når den er switchen ON så kobles den til jord, så vi får ingen lys i LED. Derfor er det HIGH state (siden LED da lyser) som skal registreres i koden, og brukes til å registrere at den er "PÅ", mens signalet egentlig er lavt
        }
      if (WhiteButtonState == HIGH) 
        {
          MyRegister[4] = 0;  // ON
        }
    }
  }
  WhitePreviousState = WhiteStateNow; 
}



void receiveEvent(int) {
  receiveFromMaster = Wire.read();
  if (S == 1)
  {
    Serial.println("RESET");
    WDT_CTRL = WDT_CTRL_ENABLE;
    //NVIC_SystemReset();
    S = 0;
  }
  if (receiveFromMaster == 2) {
    Serial.println("FtS running - Builtin LED turning on");
    U = 2;
  }
  if (receiveFromMaster == 3) {
    Serial.println("CtC running - Builtin LED turning on");
    U = 3;
  } 
} 


void requestEvent() {
  while(FtS_complete || CtC_complete) 
   {
     Serial.println("requestEvent:  ");
     if (sendToMaster == 0) {
      Serial.println("Sent 'No Signal' to Master");
      Wire.write(sendToMaster);
     }
     else if (sendToMaster == 2) {
      Serial.println("Sent 'Finished FtS' to Master");
      S = 1;
      Wire.write(sendToMaster);
      U = 0;
      sendToMaster = 0;
    }
     else if (sendToMaster == 3) {
      Serial.println("Sent 'Finished CtC' to Master");
      S = 1;
      Wire.write(sendToMaster);
      U = 0;
      sendToMaster = 0;
    }
     else if (sendToMaster == 5) {
      Serial.println("Failure");
      S = 1;
      Wire.write(sendToMaster);
      U = 0;
      sendToMaster = 0;
    }
    CtC_complete = false;
    FtS_complete = false;
  }
}


void SelectDigitz()
{
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
         EnterPassword();
         LedIndicator();
        }
     memset(Action, 0, sizeof(Action));
    }
}

void Switch()
{
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
}

void StatusCheck()
{
         while (Attempts == 0)
       {
          Serial.println("Your attempts were unsuccessful, detonation sequence initiated...");
          for (int t = 10; t > 0; t--)
           {
              Serial.print(t);
              Serial.println("...");
              DisplayBlink();
            }
             display.setSegments(SEG_BOOM);
             sendToMaster = 5;
             CtC_complete = true;
             U = 0;
             Serial.println("Bomb exploded.");
             delay(20000);
        }
}

void playCtC()
{
  while (!CtC_complete && U == 3)
  {
  StatusCheck();
  DebounceSelect();
  DebounceRight();
  DebounceLeft(); 
  data[0] = display.encodeDigit(Digit[0]);
  data[1] = display.encodeDigit(Digit[1]);
  data[2] = display.encodeDigit(Digit[2]);
  data[3] = display.encodeDigit(Digit[3]);
  display.setSegments(data);
  Switch();
  SelectDigitz();
  }

}

void playFtS()
{
 while (U == 2)
  { 
   if (Start == LOW)
    {
      UpdateSequence();
      StartButton();
    } 
   if (Start == HIGH) 
    {
     for (uint16_t counterA = 3, counterB = 0, counterC = 0; (counterA + counterB + counterC + counterD) >= 0; counterD--, SequenceCounter++) 
     { 
      UpdateSequence();    
      ProgressIndicator();
      NumbersIndicator();
      ProgressCheck();
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
           Serial.println("GAME OVER...");
           Serial.println("You ran out of time, restart the game to try again");
           matrix.clear();
           matrix.writeDisplay();
           Wire.endTransmission(0x71); 
           Wire.begin(0x79);
           delay(2000);
           sendToMaster = 5;
           FtS_complete = true;
           U = 0;
           delay(5000);
          }

          DebounceBlue();
          DebounceRed();
          DebounceGreen();
          DebounceOrange();
          DebounceWhite();

         for (int i = 0; i < 5; i++)
         {
           switch(MyRegister[i])
           {
              case 0:
                SwitchPrevious[i] = 0;
                break;

              case 1:
                if ((counterA != UnlockingSequence[i] && counterB != UnlockingSequence[i] && counterC != UnlockingSequence[i] && counterD != UnlockingSequence[i])
                    && (SwitchPrevious[i] != MyRegister[i]) && MyRegister[i] == 1)     // IF statement for when the switch is flipped at the incorrect time
                  {
                   AccelerateTimer();
                   SwitchPrevious[i] = 1;
                   memset(Progress, 0, sizeof(Progress));
                   break;
                  }
                if ((counterA == UnlockingSequence[i] || counterB == UnlockingSequence[i] || counterC == UnlockingSequence[i] || counterD == UnlockingSequence[i])
                     && (SwitchPrevious[i] != MyRegister[i]) && MyRegister[i] == 1)    // IF statement for when the switch is flipped at the correct time
                  {
                   GameProgress();
                   SwitchPrevious[i] = 1;
                   Progress[i] = 1;
                   break;
                  }

              default:
              break;
            }  
          }
      }
    }
  }
    
  }  
}
