#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "Wire.h"
#include "Adafruit_NeoPixel.h"

#define LED_COUNT_Progress  4                                               // How many NeoPixels are attached to the Arduino?
#define BRIGHTNESS 50                                                       // NeoPixel brightness, 0 (min) to 255 (max)

//set the pins for the different buttons
const int up = 2;
const int down = 3;
const int play = 4;

const int c = A0;
const int D = A1;
const int E = A2;
const int F = A3;
const int G = A4;
const int A = A5;
const int B = A6;
const int C = 0;

const int LedStripProgressPin = 5;

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

//Variables to check if all melodies is correct
int T1 = 0;
int T2 = 0;
int T3 = 0;

//Variable to know if user is done
int U = 0;
int V = 0;

//set the volume to starter value (between 0 and 30)
int volume = 15;

unsigned long debounceDelay = 30;
unsigned long lastDebounceTimeNote = 0;

Adafruit_NeoPixel stripProgress(LED_COUNT_Progress, LedStripProgressPin, NEO_RGB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

DFRobotDFPlayerMini myDFPlayer;

//Define variable to receive
byte receiveFromMaster = 0;

//Define variable to send
byte sendToMaster = 0;

//Checking to make sure that the function is actually done
boolean melodiesFinished = false;

void clearLEDs();

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

void playRtN();

void setup()
{
  Serial1.begin(9600);
  Serial.begin(115200);

  Wire.begin(0x01);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
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

  pinMode(LedStripProgressPin, OUTPUT);
  stripProgress.begin();
  stripProgress.setBrightness(BRIGHTNESS); 
  stripProgress.show();

  if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(volume);  //Set volume value to the volume int
  myDFPlayer.play(27);
}

void loop()
{ 
  debounceup();
  debouncedown();
  if (U == 1) {
    playRtN();
  }
  if (U == 6){ //startup 
    myDFPlayer.play(23);
    U = 0;
  }
  if (U == 7){ //Tick
    myDFPlayer.play(27);
    U = 0;
  }
  if (U == 8){ // Boom
    myDFPlayer.play(26);
    U = 0;
  }
  if (U == 9){ //Celebrate
    myDFPlayer.play(25);
    U = 0;
  }
  if (V == 1){
    clearLEDs();
  }
}

void clearLEDs(){
    delay(10000);
    stripProgress.clear();
    stripProgress.show();
    V = 0;
}

void receiveEvent(int) {
  receiveFromMaster = Wire.read();
  if (receiveFromMaster == 1) {
    Serial.println("RtN running");
    U = 1;
  }
  if (receiveFromMaster == 6) {
    Serial.println("Play startup");
    U = 6;
  }
  if (receiveFromMaster == 7) {
    Serial.println("Play tick sound");
    U = 7;
  }
  if (receiveFromMaster == 8) {
    Serial.println("BOOM!");
    U = 8;
  }
  if (receiveFromMaster == 9) {
    Serial.println("Woho!");
    U = 9;
  }
}

void requestEvent() {
  while(melodiesFinished) {
    if (sendToMaster == 0) {
      Serial.println("Sent 'No Signal' to Master");
      Wire.write(sendToMaster);
    }
    else if (sendToMaster == 1) {
      Serial.println("Sent 'Finished RtN' to Master");
      Wire.write(sendToMaster);
      sendToMaster = 0;
    }
    melodiesFinished = false;
    Serial.println("in request event");
  }
}

void debounceup(){
  int up_reading = digitalRead(up);

  if (up_reading != up_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (up_reading != up_state){
      up_state = up_reading;
      if (up_state == LOW){
        volume = volume + 1; //increase the volume variable
        if (volume > 30){ //check if the volume already is 30 and make sure it doesn't get higher
          volume = 30;
        }
        myDFPlayer.volume(volume); //set the volume to the new volume
        Serial.println(volume);
    }
  }
  }
  up_lastState = up_reading;
}

void debouncedown(){
  int down_reading = digitalRead(down);

  if (down_reading != down_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (down_reading != down_state){
      down_state = down_reading;
      if (down_state == LOW){
        volume = volume - 1; //decrease the volume variable
        if (volume < 0){ //check if the volume already is 0 and make sure it doesn't get negative
          volume = 0;
        }
        myDFPlayer.volume(volume); //set the volume to the new volume
        Serial.println(volume);
    }
  }
  }
  down_lastState = down_reading;
}

void debounceplay(){
  int play_reading = digitalRead(play);

  if (play_reading != play_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (play_reading != play_state){
      play_state = play_reading;
      if (play_state == LOW){
        if (T1 == 0 and T2 == 0){ //check if the first melody is done
          myDFPlayer.play(13); //play the first melody again
        }
        else if (T1 == 1 and T2 == 0){ //check if the second melody is done
          myDFPlayer.play(14); //play the second melody again
        }
        else if (T1 == 1 and T2 == 1){ //check if the third melody is done
          myDFPlayer.play(19); //play the third melody again
        }
     }
    }
  }
  play_lastState = play_reading;
}

void debouncec(){ 
  int c_reading = digitalRead(c);

  if (c_reading != c_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (c_reading != c_state){
      c_state = c_reading;
      if (c_state == LOW){
        myDFPlayer.play(1); //play the c note
        if (R1 == 1 and R2 == 0){ //check if user press the correct button combination
          R2 = 1;
        }
        else if (R13 == 1 and R14 == 0){
          R14 = 1;
        }
        else{
          R1 = 1, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  c_lastState = c_reading;
}

void debounceD(){
  int D_reading = digitalRead(D);

  if (D_reading != D_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (D_reading != D_state){
      D_state = D_reading;
      if (D_state == LOW){
      myDFPlayer.play(2); //play the D note
        if (R11 == 1 and R12 == 0){ //check if user press the correct button combination
          R12 = 1; 
        }
        else if (R12 == 1 and R13 == 0){
          R13 = 1;
        } 
        else if (Z12 == 1 and Z13 == 0){
          Z13 = 1;
        }
        else if (S7 == 1 and S8 == 0){
          S8 = 1;
        }
        else{
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  D_lastState = D_reading;
}

void debounceE(){
  int E_reading = digitalRead(E);

  if (E_reading != E_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (E_reading != E_state){
      E_state = E_reading;
      if (E_state == LOW){
        myDFPlayer.play(3); //play the E note
        if (R9 == 1 and R10 == 0){ //check if user press the correct button combination
          R10 = 1;
        }
        else if (R10 == 1 and R11 == 0){
          R11 = 1;
        }
        else if (Z8 == 1 and Z9 == 0){
          Z9 = 1;
        }
        else if (Z11 == 1 and Z12 == 0){
          Z12 = 1;
        }
        else{
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  E_lastState = E_reading;
}

void debounceF(){
  int F_reading = digitalRead(F);

  if (F_reading != F_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (F_reading != F_state){
      F_state = F_reading;
      if (F_state == LOW){
        myDFPlayer.play(4); //play the F note
        if (R7 == 1 and R8 == 0){ //check if user press the correct button combination
          R8 = 1;
        }
        else if (R8 == 1 and R9 == 0){
          R9 = 1;
        }
        else if (Z7 == 1 and Z8 == 0){
          Z8 = 1;
        }
        else if (Z9 == 1 and Z10 == 0){
          Z10 = 1;
        }
        else{
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  F_lastState = F_reading;
}

void debounceG(){
  int G_reading = digitalRead(G);

  if (G_reading != G_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (G_reading != G_state){
      G_state = G_reading;
      if (G_state == LOW){
        myDFPlayer.play(5); //play the G note
        if (R2 == 1 and R3 == 0){ //check if user press the correct button combination
          R3 = 1;
        }
        else if (R3 == 1 and R4 == 0){
          R4 = 1;
        }
        else if (R6 == 1 and R7 == 0){
          R7 = 1;
        }
        else if (Z5 == 1 and Z6 == 0){
          Z6 = 1;
        }
        else if (Z10 == 1 and Z11 == 0){
          Z11 = 1;
        }
        else if (S1 == 1 and S2 == 0){
          S2 = 1;
        }
        else if (S4 == 1 and S5 == 0){
          S5 = 1;
        }
        else if (S8 == 1 and S9 == 0){
          S9 = 1;
        }
        else if (S9 == 1 and S10 == 0){
          S10 = 1;
        }
        else if (S12 == 1 and S13 == 0){
          S13 = 1;
        }
        else{
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 1, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  G_lastState = G_reading;
}

void debounceA(){
  int A_reading = digitalRead(A);

  if (A_reading != A_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (A_reading != A_state){
      A_state = A_reading;
      if (A_state == LOW){
        myDFPlayer.play(6); //play the A note
        if (R4 == 1 and R5 == 0){ //check if user press the correct button combination
          R5 = 1;
        }
        else if (R5 == 1 and R6 == 0){
        R6 = 1;
        }
        else if (S2 == 1 and S3 == 0){
          S3 = 1;
        }
        else if (S6 == 1 and S7 == 0){
          S7 = 1;
        }
        else if (S10 == 1 and S11 == 0){
          S11 = 1;
        }
        else{
          R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
          S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
          Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  A_lastState = A_reading;
}

void debounceB(){
  int B_reading = digitalRead(B);

  if (B_reading != B_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (B_reading != B_state){
      B_state = B_reading;
      if (B_state == LOW){
        myDFPlayer.play(7); //play the B note
        if (Z1 == 1 and Z2 == 0){ //check if user press the correct button combination
          Z2 = 1;
        }
        else if (Z3 == 1 and Z4 == 0){
          Z4 = 1;
        }
        else if (S3 == 1 and S4 == 0){
          S4 = 1;
        }
        else if (S5 == 1 and S6 == 0){
          S6 = 1;
        }
        else if (S11 == 1 and S12 == 0){
          S12 = 1;
        }
        else{
        R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
        S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
        Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  B_lastState = B_reading;
}

void debounceC(){
  int C_reading = digitalRead(C);

  if (C_reading != C_lastState){
     lastDebounceTimeNote = millis();
  }

  if ((millis() - lastDebounceTimeNote) > debounceDelay){
    if (C_reading != C_state){
      C_state = C_reading;
      if (C_state == LOW){
        myDFPlayer.play(8); //play the C note
        if (Z2 == 1 and Z3 == 0){ //check if user press the correct button combination
          Z3 = 1;
        }
        else if (Z4 == 1 and Z5 == 0){
          Z5 = 1;
        }
        else if (Z6 == 1 and Z7 == 0){
          Z7 = 1;
        }
        else{
        R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0;
        S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0;
        Z1 = 1, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0;
        }
      }
    }
  }
  C_lastState = C_reading;
}

void twinkle(){
  if (R1 == 1 && R2 == 1 && R3 == 1 && R4 == 1 && R5 == 1 && R6 == 1 && R7 == 1 && R8 == 1 && R9 == 1 && R10 == 1 && R11 == 1 && R12 == 1 && R13 == 1 && R14 == 1){ //Check if the first button combination is completed
    delay(1000);
    stripProgress.setPixelColor(0, 0xFF0000);
    stripProgress.show();
    myDFPlayer.play(20); //play the melody to show that the user input the correct combination, with the new song at the end
    R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0, R13 = 0, R14 = 0; //reset the combination for the first melody
    T1 = 1; //Variable to check if all melodies is completed
    }
  }

void yankee(){
  if (S1 == 1 && S2 == 1 && S3 == 1 && S4 == 1 && S5 == 1 && S6 == 1 && S7 == 1 && S8 == 1 && S9 == 1 && S10 == 1 && S11 == 1 && S12 == 1 && S13 == 1){ //Check if the second combination is completed
    delay(1000);
    stripProgress.setPixelColor(1, 0xFF0000);
    stripProgress.show();
    myDFPlayer.play(21); //play the melody to show that the user input the correct combination, with the new song at the end
    S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, S7 = 0, S8 = 0, S9 = 0, S10 = 0, S11 = 0, S12 = 0, S13 = 0; //reset the combination for the second melody
    T2 = 1; //Variable to check if all melodies is completed
    }
  }

void river(){
  if (Z1 == 1 && Z2 == 1 && Z3 == 1 && Z4 == 1 && Z5 == 1 && Z6 == 1 && Z7 == 1 && Z8 == 1 && Z9 == 1 && Z10 == 1 && Z11 == 1 && Z12 == 1 && Z13 == 1){//Check if the third combination is completed
    delay(1000);
    stripProgress.setPixelColor(2, 0xFF0000);
    stripProgress.setPixelColor(3, 0xFF0000);
    stripProgress.show();
    myDFPlayer.play(18); //play the melody to show that the user input the correct combination
    Z1 = 0, Z2 = 0, Z3 = 0, Z4 = 0, Z5 = 0, Z6 = 0, Z7 = 0, Z8 = 0, Z9 = 0, Z10 = 0, Z11 = 0, Z12 = 0, Z13 = 0; //reset the combination for the third melody
    T3 = 1; //Variable to check if all melodies is completed
    }
  }

void total(){
  if (T1 == 1 && T2 == 1 && T3 == 1){ //checks if the user has done all three melodies
    T1 = 0, T2 = 0, T3 = 0; //reset the checks for the three melodies
    U = 0; //when U = 0 the program knows that the user is done with this module
    V = 1;
    melodiesFinished = true;
    sendToMaster = 1;
    }
  }

void playRtN(){
  myDFPlayer.play(13);  //Play the first melody
  while (!melodiesFinished){
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
  }
}