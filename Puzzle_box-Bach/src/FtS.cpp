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



#define LED_COUNT_Progress  5                                               // How many NeoPixels are attached to the Arduino?
#define LED_COUNT_Numbers  4                                                // How many NeoPixels are attached to the Arduino?
#define BRIGHTNESS 50                                                       // NeoPixel brightness, 0 (min) to 255 (max)

const int           BlueButtonPin           = 0;                            // Blue ToggleSwitch pin
const int           RedButtonPin            = 1;                            // Red ToggleSwitch pin
const int           GreenButtonPin          = 2;                            // Green ToggleSwitch pin
const int           OrangeButtonPin         = 3;                            // Orange ToggleSwitch pin
const int           WhiteButtonPin          = 4;                            // White ToggleSwitch pin
const int           LedStripProgressPin     = A1;                           // Data in Pin for the Led Strip indicator
const int           LedStripNumbersPin      = A2;                           // Data in Pin for the Led Strip indicator
const int           MagneticButtonPin       = A3;                           // Data in Pin from magnetic push button
const int           StartButtonPin          = A4;                           // Data in pin Start button


//int                 MagneticPuzzle          = HIGH;                       // A magnetic button determines if the game can be started (start button cant be used as a reset after the game has started)
int                 Allow_Start             = LOW;                          // Used as a variable to make it so that a player cant restart the game after losing 
int                 Start                   = LOW; 
int                 MyRegister[5]           = {0, 0, 0, 0, 0};              // Saves the states of toggle switches in an array
int                 Progress[5]             = {0, 0, 0, 0, 0};              // Saves the progress in an array
int                 SwitchPrevious[5]       = {0, 0, 0, 0, 0};              // Savess the previous state of the toggle switches for the Switch-Case
int                 UnlockingSequence[5]    = {8, 3, 5, 9, 7};              // This array gives the numbers on the timer where individual switches can be toggled to get a "pass", UnlockingSequence[0] is for the blue toggle switch etc..
int                 SequenceCounter         = 0;                            // Update UnlockingSequence every 15 second
int                 PreviousNumber          = 0;

// variables will change:
int                 BlueButtonState         = LOW;                          // variable for reading the toggle switch status
int                 RedButtonState          = LOW;                          // variable for reading the toggle switch status
int                 GreenButtonState        = LOW;                          // variable for reading the toggle switch status
int                 OrangeButtonState       = LOW;                          // variable for reading the toggle switch status
int                 WhiteButtonState        = LOW;                          // variable for reading the toggle switch status

int                 BlueStateNow            = 0;
int                 RedStateNow             = 0;
int                 GreenStateNow           = 0;
int                 OrangeStateNow          = 0;
int                 WhiteStateNow           = 0;

//boolean buttonState = LOW;
int                 BluePreviousState       = LOW;                           // Saves the previous state of the toggle switch 
int                 RedPreviousState        = LOW;                           // Saves the previous state of the toggle switch
int                 GreenPreviousState      = LOW;                           // Saves the previous state of the toggle switch
int                 OrangePreviousState     = LOW;                           // Saves the previous state of the toggle switch
int                 WhitePreviousState      = LOW;                           // Saves the previous state of the toggle switch 


unsigned long       DebounceDelay           = 50;                                                         // Debounce delay for toggle switches
unsigned long       DebounceTimer           = 0;                                                          // Stores the current value of millis()
unsigned long       LoopDelay               = 1000;                                                       // ticks down starting at 1000ms rate (normal speed)
unsigned long       LoopTimer               = 0;                                                          // Stores the current value of millis()
unsigned long       PassDebounceTimer       = 0;                                                          // Stores the current value of millis()        
unsigned long       PassDebounceDelay       = 50;                                                         // Debounce delay after a successful toggle 
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

void ProgressCheck()
{
if ((Progress[0] == 1 && Progress[1] == 1 && Progress[2] == 1 && Progress[3] == 1 && Progress[4] == 1) && (MyRegister[0] == 1 && MyRegister[1] == 1 && MyRegister[2] == 1 && MyRegister[3] == 1 && MyRegister[4] == 1))
  {
    Serial.println("Congratulations, you beat the game!");
    matrix.print("DONE");
    matrix.writeDisplay(); 
    Allow_Start = HIGH;

  }
}

void StartButton()
{
  if (digitalRead(StartButtonPin) == HIGH)   //&& digitalRead(MagneticButtonPin) == LOW
    {
     Start = HIGH;
     Serial.println("starting..");
     //LoopDelay = LoopDelay + 100; 
    }
}

void NumbersIndicator()
{
  int x = counterD;
  if (x >= 5) {x = 0;}
  if (x != PreviousNumber)                                 // Første LED fra Din er MSB i den binære koden som vises
     {
          switch(UnlockingSequence[x])
           {
              case 0:
              stripNumbers.setPixelColor(0, 0); 
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, 0);
              break;

              case 1:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, BRGOW[x]);
              break;

              case 2:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, BRGOW[x]);
              stripNumbers.setPixelColor(3, 0);
              break;

              case 3:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, BRGOW[x]);
              stripNumbers.setPixelColor(3, BRGOW[x]);
              break;

              case 4:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, BRGOW[x]);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, 0);
              break;

              case 5:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, BRGOW[x]);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, BRGOW[x]);
              break;

              case 6:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, BRGOW[x]);
              stripNumbers.setPixelColor(2, BRGOW[x]);
              stripNumbers.setPixelColor(3, 0);
              break;

              case 7:
              stripNumbers.setPixelColor(0, 0);
              stripNumbers.setPixelColor(1, BRGOW[x]);
              stripNumbers.setPixelColor(2, BRGOW[x]);
              stripNumbers.setPixelColor(3, BRGOW[x]);
              break;

              case 8:
              stripNumbers.setPixelColor(0, BRGOW[x]);
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, 0);
              break;

              case 9:
              stripNumbers.setPixelColor(0, BRGOW[x]);
              stripNumbers.setPixelColor(1, 0);
              stripNumbers.setPixelColor(2, 0);
              stripNumbers.setPixelColor(3, BRGOW[x]);
              break;

              default:
              break;
            }
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
  if (SequenceCounter > 30)
  {
    for (int u = 0, m = 5; u < 5; u++, m--)
    {
      UnlockingSequence[u] = (UnlockingSequence[u] - m);
      if (UnlockingSequence[u] > 9){UnlockingSequence[u] = 1;}
      if (UnlockingSequence[u] < 1){UnlockingSequence[u] = 7;}
    } 
    SequenceCounter = 0;
    Serial.println(" ");
    for (int s = 0; s < 5; s++)
    {
      Serial.print(UnlockingSequence[s]);
    }
    Serial.println("Unlocking sequence UPDATED");
  }
}

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
    LoopDelay = (LoopDelay - 25); // Timer ticks down at a faster rate after failure (wrong input)
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
pinMode(BlueButtonPin, INPUT_PULLUP);        // Initialize Toggle switch pins as NC
pinMode(RedButtonPin, INPUT_PULLUP);
pinMode(GreenButtonPin, INPUT_PULLUP);
pinMode(OrangeButtonPin, INPUT_PULLUP);
pinMode(WhiteButtonPin, INPUT_PULLUP);
pinMode(MagneticButtonPin, INPUT_PULLUP);    // Initialize Magnetic button pin A3 -> MagneticPuzzle NC starts HIGH (INPUT_PULLUP), while MagneticButton is being pressed INPUT stays LOW, and thus MagneticPuzzle variable is LOW which will momentarily enable the StartButton
pinMode(StartButtonPin, INPUT_PULLUP);       // Initialize Start button        A4 -> Allow_Start NC starts LOW (INPUT_PULLUP, when INPUT goes HIGH from PushButton, Allow_Start is set to HIGH)
pinMode(LedStripProgressPin, OUTPUT);        // Initialize LED strip pins as output
pinMode(LedStripNumbersPin, OUTPUT);

stripProgress.begin();                       // INITIALIZE NeoPixel strip object (REQUIRED)
stripNumbers.begin();                        // INITIALIZE NeoPixel strip object (REQUIRED)
stripProgress.show();                        // Turn OFF all pixels ASAP
stripNumbers.show();                         // Turn OFF all pixels ASAP
stripProgress.setBrightness(BRIGHTNESS); 
stripNumbers.setBrightness(BRIGHTNESS);
}


void loop() 
{
  StartButton();
  while (Allow_Start == HIGH)
  {
    Serial.println("Can you figure out how to start the game?");
    matrix.writeDisplay();
  }

 while (Allow_Start == LOW && Start == HIGH) //== HIGH && digitalRead(MagneticButtonPin) == LOW
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
           Allow_Start = HIGH;
           break;
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
                if ((counterA != UnlockingSequence[i] && counterB != UnlockingSequence[i] && counterC != UnlockingSequence[i] && counterD != UnlockingSequence[i]) && (SwitchPrevious[i] != MyRegister[i]) && MyRegister[i] == 1)     // IF statement for when the switch is flipped at the incorrect time
                  {
                   AccelerateTimer();
                   SwitchPrevious[i] = 1;
                   memset(Progress, 0, sizeof(Progress));
                   break;
                  }
                if ((counterA == UnlockingSequence[i] || counterB == UnlockingSequence[i] || counterC == UnlockingSequence[i] || counterD == UnlockingSequence[i]) && (SwitchPrevious[i] != MyRegister[i]) && MyRegister[i] == 1)
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

