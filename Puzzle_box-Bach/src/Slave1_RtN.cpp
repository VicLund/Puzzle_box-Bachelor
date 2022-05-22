// Updated code can be found here: https://github.com/VicLund/Puzzle_box-Bachelor/tree/main/Puzzle_box-Bach/src

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "Wire.h"
#include "Adafruit_NeoPixel.h"

#define LED_COUNT_Progress  4 // How many NeoPixels are attached to the Arduino?
#define BRIGHTNESS 50 // NeoPixel brightness, 0 (min) to 255 (max)

//Data pins for buttons
const int up = 2;
const int play = 3;
const int down = 4;

const int c = A1;
const int D = A2;
const int E = A3;
const int F = A4;
const int G = A5;
const int A = A6;
const int B = 0;
const int C = 1;

//Data pins for LEDs
const int LedStripProgressPin = A0;

//Set the last button states to LOW
int up_state;
int up_lastState = LOW;
int down_state;
int down_lastState = LOW;
int play_state;
int play_lastState = LOW;

int c_state;
int c_lastState = LOW;
int D_state;
int D_lastState = LOW;
int E_state;
int E_lastState = LOW;
int F_state;
int F_lastState = LOW;
int G_state;
int G_lastState = LOW;
int A_state;
int A_lastState = LOW;
int B_state;
int B_lastState = LOW;
int C_state;
int C_lastState = LOW;

//Variables for first melody check
int R1 = 0;
int R2 = 0;
int R3 = 0;
int R4 = 0;
int R5 = 0;
int R6 = 0;
int R7 = 0;
int R8 = 0;
int R9 = 0;
int R10 = 0;
int R11 = 0;
int R12 = 0;
int R13 = 0;
int R14 = 0;

//Variables for second melody check
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;
int S5 = 0;
int S6 = 0;
int S7 = 0;
int S8 = 0;
int S9 = 0;
int S10 = 0;
int S11 = 0;
int S12 = 0;
int S13 = 0;

//Variables for third melody check
int Z1 = 0;
int Z2 = 0;
int Z3 = 0;
int Z4 = 0;
int Z5 = 0;
int Z6 = 0;
int Z7 = 0;
int Z8 = 0;
int Z9 = 0;
int Z10 = 0;
int Z11 = 0;
int Z12 = 0;
int Z13 = 0;

//Variables for giving up
int G1 = 0;
int G2 = 0;
int G3 = 0;
int G4 = 0;
int G5 = 0;
int G6 = 0;
int G7 = 0;
int G8 = 0;

//Variables to check if all melodies is correct
int T1 = 0;
int T2 = 0;
int T3 = 0;

//Variable to know if user is done
int U = 0;

//Variable for delaying LED turnoff
int V = 0;

//set the volume to starter value (between 0 and 30)
int volume = 20;

//Debounce variables
unsigned long debounceDelay = 30;
unsigned long lastDebounceTimeNote = 0;

//Variables for LED strip
Adafruit_NeoPixel stripProgress(LED_COUNT_Progress, LedStripProgressPin, NEO_RGB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

//Renaming the DFRobotDFPlayerMini to an easier name
DFRobotDFPlayerMini myDFPlayer;

//Define variable to receive
byte receiveFromMaster = 0;

//Define variable to send
byte sendToMaster = 0;

//Checking to make sure that the function is actually done
boolean melodiesFinished = false;

//Forward declaring the functions
void clearLEDs();
void clearLed();

void receiveEvent(int);
void requestEvent();

void debounceup();
void debouncedown();
void debounceplay();

void debouncec();
void debounceD();
void debounceE();
void debounceF();
void debounceG();
void debounceA();
void debounceB();
void debounceC();

void twinkle();
void yankee();
void river();
void total();

void giveup();

void playRtN();

void setup()
{
  Serial1.begin(9600); //For communicationg between the arduino and DFPlayer
  Serial.begin(115200); //For communicating between the arduino and PC
  
  Wire.begin(0x78); //I2C, communicating through address 0x78
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //Set all buttons to Pullup
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(play, INPUT_PULLUP);

  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(c, INPUT_PULLUP);
  pinMode(D, INPUT_PULLUP);
  pinMode(E, INPUT_PULLUP);
  pinMode(F, INPUT_PULLUP);
  pinMode(G, INPUT_PULLUP);
  pinMode(C, INPUT_PULLUP);

  // set the LED pin to output, initializing the LEDs and set the brightness
  pinMode(LedStripProgressPin, OUTPUT);
  stripProgress.begin();
  stripProgress.setBrightness(BRIGHTNESS);
  stripProgress.show();

  if (!myDFPlayer.begin(Serial1)) {  //Communicate with DFPlayer
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online.")); //Print that the DFPlayer is done with setup
  myDFPlayer.volume(volume);  //Set volume on the DFPlayer to the value of the volume int
}

void loop()
{
  debounceup(); //Make volume up button available even when not playing RtN
  debouncedown(); //Make volume down button available even when not playing RtN
  if (U == 1) {
    playRtN(); //If U = 1, play the module RtN
  }
  if (U == 6) { //If U = 6, play the startup sound
    myDFPlayer.play(23); //play the song on the SD card with number 0023
    U = 0;
  }
  if (U == 7) { //If U = 7, play the ticking sound
    myDFPlayer.play(27); //play the song on the SD card with number 0027
    U = 0;
  }
  if (U == 8) { //If U = 8, play the "Boom" sound
    myDFPlayer.play(25); //play the song on the SD card with number 0025
    U = 0;
  }
  if (U == 9) { //If U = 9, play the victory sound
    myDFPlayer.play(26); //play the song on the SD card with number 0026
    U = 0;
  }
  if (V == 1) {
    clearLEDs(); //If V = 1, clear leds with a 10 second delay
  }
  if (V == 2) {
    clearLED(); //If V = 2, clear leds with a 5 second delay
  }
}

void clearLEDs() {
  delay(10000); //Wait for 10 seconds
  stripProgress.clear(); //clear the Leds
  stripProgress.show();
  V = 0; // Set V back to 0
}

void clearLED() {
  delay(5000); //Wait for 5 seconds
  stripProgress.clear(); //clear the Leds
  stripProgress.show();
  V = 0; // Set V back to 0
}

void receiveEvent(int) {
  receiveFromMaster = Wire.read(); //Set receiveFromMaster to the value received from I2C
  if (receiveFromMaster == 1) {
    U = 1; //If 1 is received, set U to 1 to start the PlayRtN();
  }
  if (receiveFromMaster == 6) {
    U = 6; //If 6 is received, set U to 6 to play the startup sound
  }
  if (receiveFromMaster == 7) {
    U = 7; //If 7 is received, set U to 7 to play the ticking sound
  }
  if (receiveFromMaster == 8) {
    U = 8; //If 8 is received, set U to 8 to play the "Boom" sound
  } 
  if (receiveFromMaster == 9) {
    U = 9; //If 9 is received, set U to 9 to play the victory sound
  }
}

void requestEvent() {
  while (melodiesFinished) { // While the melodiesFinished is True
    if (sendToMaster == 0) {
      Wire.write(sendToMaster); //
    }
    else if (sendToMaster == 1) {
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    else if (sendToMaster == 10) {
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    melodiesFinished = false;
    sendToMaster = 0;
  }
}

void debounceup() {
  int up_reading = digitalRead(up);

  if (up_reading != up_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (up_reading != up_state) {
      up_state = up_reading;
      if (up_state == LOW) {
        volume = volume + 1; //increase the volume variable
        if (volume > 30) { //check if the volume already is 30 and make sure it doesn't get higher
          volume = 30;
        }
        myDFPlayer.volume(volume); //set the volume to the new volume
      }
    }
  }
  up_lastState = up_reading;
}

void debouncedown() {
  int down_reading = digitalRead(down);

  if (down_reading != down_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (down_reading != down_state) {
      down_state = down_reading;
      if (down_state == LOW) {
        volume = volume - 1; //decrease the volume variable
        if (volume < 0) { //check if the volume already is 0 and make sure it doesn't get negative
          volume = 0;
        }
        myDFPlayer.volume(volume); //set the volume to the new volume
      }
    }
  }
  down_lastState = down_reading;
}

void debounceplay() {
  int play_reading = digitalRead(play);
  if (play_reading != play_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (play_reading != play_state) {
      play_state = play_reading;
      if (play_state == LOW) {
        if (T1 == 0 and T2 == 0) { //check if the first/second melody is done
          myDFPlayer.play(13); //play the first melody again
        }
        else if (T1 == 1 and T2 == 0) { //check if the first/second melody is done
          myDFPlayer.play(14); //play the second melody again
        }
        else if (T1 == 1 and T2 == 1) { //check if the first/second melody is done
          myDFPlayer.play(19); //play the third melody again
        }
      }
    }
  }
  play_lastState = play_reading;
}

void debouncec() {
  int c_reading = digitalRead(c);

  if (c_reading != c_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (c_reading != c_state) {
      c_state = c_reading;
      if (c_state == LOW) {
        myDFPlayer.play(1); //play the c note, the file on the SD card with name 0001
        if (R1 == 1 and R2 == 0) { //if c is pressed after R1 = 1, set R2 = 1
          R2 = 1;
          G1 = 1, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
          S1 = 0;
        }
        else if (R13 == 1 and R14 == 0) { //if c is pressed after R13 = 1, set R14 = 1
          R14 = 1;
          G1 = 1, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
        else if (R1 == 1 and R2 == 1 and R3 == 0){
          G1 = 1, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
        else { //First/Wrong button, reset all variables
          R1 = 1, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 1, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  c_lastState = c_reading;
}

void debounceD() {
  int D_reading = digitalRead(D);

  if (D_reading != D_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (D_reading != D_state) {
      D_state = D_reading;
      if (D_state == LOW) {
        myDFPlayer.play(2); //play the D note
        if (R11 == 1 and R12 == 0) { //if D is pressed after R11 = 1, set R12 = 1
          R12 = 1;
        }
        else if (R12 == 1 and R13 == 0) { //if D is pressed after R12 = 1, set R13 = 1
          R13 = 1;
        }
        else if (Z12 == 1 and Z13 == 0) { //if D is pressed after Z12 = 1, set Z13 = 1
          Z13 = 1;
        }
        else if (S7 == 1 and S8 == 0) { //if D is pressed after S7 = 1, set S8 = 1
          S8 = 1;
        }
        else if (G2 == 1 and G3 == 0){ //if D is pressed after G2 = 1, set G3 = 1
          G3 = 1;
          R3 = 0;
          S2 = 0;
        }
        else { //Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  D_lastState = D_reading;
}

void debounceE() {
  int E_reading = digitalRead(E);

  if (E_reading != E_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (E_reading != E_state) {
      E_state = E_reading;
      if (E_state == LOW) {
        myDFPlayer.play(3); //play the E note
        if (R9 == 1 and R10 == 0) { //check if user press the correct button combination
          R10 = 1;
        }
        else if (R10 == 1 and R11 == 0) {
          R11 = 1;
        }
        else if (Z8 == 1 and Z9 == 0) {
          Z9 = 1;
        }
        else if (Z11 == 1 and Z12 == 0) {
          Z12 = 1;
        }
        else if (G4 == 1 and G5 == 0){
          G5 = 1;
        }
        else { //Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  E_lastState = E_reading;
}

void debounceF() {
  int F_reading = digitalRead(F);

  if (F_reading != F_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (F_reading != F_state) {
      F_state = F_reading;
      if (F_state == LOW) {
        myDFPlayer.play(4); //play the F note
        if (R7 == 1 and R8 == 0) { //check if user press the correct button combination
          R8 = 1;
        }
        else if (R8 == 1 and R9 == 0) {
          R9 = 1;
        }
        else if (Z7 == 1 and Z8 == 0) {
          Z8 = 1;
        }
        else if (Z9 == 1 and Z10 == 0) {
          Z10 = 1;
        }
        else if (G6 == 1 and G7 == 0){
          G7 = 1;
        }
        else { //Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  F_lastState = F_reading;
}

void debounceG() {
  int G_reading = digitalRead(G);

  if (G_reading != G_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (G_reading != G_state) {
      G_state = G_reading;
      if (G_state == LOW) {
        myDFPlayer.play(5); //play the G note        
        if (R2 == 1 and R3 == 0) { //check if user press the correct button combination
          R3 = 1;
          S1 = 1;
          G2 = 1;
        }
        else if (R3 == 1 and R4 == 0) {
          R4 = 1;
          S2 = 1;
          G1 = 0, G2 = 0;
        }
        else if (R6 == 1 and R7 == 0) {
          R7 = 1;
          S1 = 1;
        }
        else if (Z5 == 1 and Z6 == 0) {
          Z6 = 1;
          S1 = 1;
        }
        else if (Z10 == 1 and Z11 == 0) {
          Z11 = 1;
          S1 = 1;
        }
        else if (S1 == 1 and S2 == 0) {
          S2 = 1, G1 = 0, G2 = 0;
        }
        else if (S1 == 1 and S2 == 1 and S3 == 0){
          S2 = 1, G1 = 0, G2 = 0;
        }
        else if (S4 == 1 and S5 == 0) {
          S5 = 1;
        }
        else if (S8 == 1 and S9 == 0) {
          S9 = 1;
        }
        else if (S9 == 1 and S10 == 0) {
          S10 = 1;
        }
        else if (S12 == 1 and S13 == 0) {
          S13 = 1;
        }
        else if (G1 == 1 and G2 == 0){
          G2 = 1;
          S1 = 1;
        }
        else {//First/Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 1, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  G_lastState = G_reading;
}

void debounceA() {
  int A_reading = digitalRead(A);

  if (A_reading != A_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (A_reading != A_state) {
      A_state = A_reading;
      if (A_state == LOW) {
        myDFPlayer.play(6); //play the A note
        if (R4 == 1 and R5 == 0) { //check if user press the correct button combination
          R5 = 1, S3 = 1;
        }
        else if (R5 == 1 and R6 == 0) {
          R6 = 1, S3 = 0, S2 = 0, S1 = 0;
        }
        else if (S2 == 1 and S3 == 0 and G3 == 1 and G4 == 0) {
          S3 = 1, G4 = 1;
        }
        else if (S2 == 1 and S3 == 0) {
          S3 = 1;
        }
        else if (S6 == 1 and S7 == 0) {
          S7 = 1;
        }
        else if (S10 == 1 and S11 == 0) {
          S11 = 1;
        }
        else if (G3 == 1 and G4 == 0){
          G4 = 1;
        }
        else { //Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  A_lastState = A_reading;
}

void debounceB() {
  int B_reading = digitalRead(B);

  if (B_reading != B_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (B_reading != B_state) {
      B_state = B_reading;
      if (B_state == LOW) {
        myDFPlayer.play(7); //play the B note
        if (Z1 == 1 and Z2 == 0) { //check if user press the correct button combination
          Z2 = 1;
        }
        else if (Z3 == 1 and Z4 == 0) {
          Z4 = 1;
        }
        else if (S3 == 1 and S4 == 0) {
          S4 = 1, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0;
        }
        else if (S5 == 1 and S6 == 0) {
          S6 = 1;
        }
        else if (S11 == 1 and S12 == 0) {
          S12 = 1;
        }
        else if (G5 == 1 and G6 == 0){
          G6 = 1;
        }
        else { //Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  B_lastState = B_reading;
}

void debounceC() {
  int C_reading = digitalRead(C);

  if (C_reading != C_lastState) {
    lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay) {
    if (C_reading != C_state) {
      C_state = C_reading;
      if (C_state == LOW) {
        myDFPlayer.play(8); //play the C note
        if (Z2 == 1 and Z3 == 0) { //check if user press the correct button combination
          Z3 = 1;
        }
        else if (Z4 == 1 and Z5 == 0) {
          Z5 = 1;
        }
        else if (Z6 == 1 and Z7 == 0) {
          Z7 = 1;
        }
        else if (G7 == 1 and G8 == 0){
          G8 = 1;
          Z1 = 1;
        }
        else { //First/Wrong button, reset all variables
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 1, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
          G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0;
        }
      }
    }
  }
  C_lastState = C_reading;
}

void twinkle() {
  if (R1 == 1 && R2 == 1 && R3 == 1 && R4 == 1 && R5 == 1 && R6 == 1 && R7 == 1 && R8 == 1 && R9 == 1 && R10 == 1 && R11 == 1 && R12 == 1 && R13 == 1 && R14 == 1) { //Check if the first button combination is completed
    delay(500); //Wait for the last pressed note to finish playing
    stripProgress.setPixelColor(3, 0xFF0000); //Set the first LED to green
    stripProgress.show(); //Show the LED
    myDFPlayer.play(20); //play the melody to show that the user input the correct combination, with the new song at the end
    R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0; //reset the combination for the first melody
    T1 = 1; //Variable to check if all melodies is completed
  }
}

void yankee() {
  if (S1 == 1 && S2 == 1 && S3 == 1 && S4 == 1 && S5 == 1 && S6 == 1 && S7 == 1 && S8 == 1 && S9 == 1 && S10 == 1 && S11 == 1 && S12 == 1 && S13 == 1) { //Check if the second combination is completed
    delay(500); //Wait for the last pressed note to finish playing
    stripProgress.setPixelColor(2, 0xFF0000); //Set the second LED to green
    stripProgress.show(); //Show the LED
    myDFPlayer.play(21); //play the melody to show that the user input the correct combination, with the new song at the end
    S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0; //reset the combination for the second melody
    T2 = 1; //Variable to check if all melodies is completed
  }
}

void river() {
  if (Z1 == 1 && Z2 == 1 && Z3 == 1 && Z4 == 1 && Z5 == 1 && Z6 == 1 && Z7 == 1 && Z8 == 1 && Z9 == 1 && Z10 == 1 && Z11 == 1 && Z12 == 1 && Z13 == 1) { //Check if the third combination is completed
    delay(500); //Wait for the last pressed note to finish playing
    stripProgress.setPixelColor(1, 0xFF0000); //Set the third LED to green
    stripProgress.setPixelColor(0, 0xFF0000); //Set the fourth LED to green
    stripProgress.show(); //Show the LEDs
    myDFPlayer.play(18); //play the melody to show that the user input the correct combination
    Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0; //reset the combination for the third melody
    T3 = 1; //Variable to check if all melodies is completed
  }
}

void total() {
  if (T1 == 1 && T2 == 1 && T3 == 1) { //checks if the user has done all three melodies
    T1 = 0, T2 = 0, T3 = 0; //reset the checks for the three melodies
    U = 0; //when U = 0 the program knows that the user is done with this module
    V = 1; //set V = 1 to add the delay before the LEDs turn off
    melodiesFinished = true; //Set melodiesFinished to true, so the program goes into the while loop in requestEvent
    sendToMaster = 1; //Set sendToMaster to 1 to send that value to the Master
  }
}

void giveup(){
  if (G1 == 1 && G2 == 1 && G3 == 1 && G4 == 1 && G5 == 1 && G6 == 1 && G7 == 1 && G8 == 1){
    G1 = 0, G2 = 0, G3 = 0, G4 = 0, G5 = 0, G6 = 0, G7 = 0, G8 = 0; //reset the checks for the give up sequence
    U = 0; //when U = 0 the program knows that the user is done with this module
    V = 2; //set V = 2 to add the delay before the LEDs turn off
    melodiesFinished = true; //Set melodiesFinished to true, so the program goes into the while loop in requestEvent
    sendToMaster = 10; //Set sendToMaster to 10 to send that value to the Master
    stripProgress.fill(0x00FF00), 0, LED_COUNT_Progress; // Set all LEDs to red
    stripProgress.show(); //show the Leds
  }
}

void playRtN() { //When U = 1 (Received 1 from master), the program goes into this function
  myDFPlayer.play(13);  //Play the first melody that the user should try to play
  while (!melodiesFinished) {
    debounceup(); //button for volume up
    debouncedown(); //button for volume down
    debounceplay(); //button for replaying the melody

    debouncec(); //button for low c note
    debounceD(); //button for D note
    debounceE(); //button for E note
    debounceF(); //button for F note
    debounceG(); //button for G note
    debounceA(); //button for A note
    debounceB(); //button for B note
    debounceC(); //button for high C note

    twinkle(); //check for first melody
    yankee(); //check for second melody
    river(); //check for third melody
    total(); //check for all melodies

    giveup(); //check for giving up
  }
}