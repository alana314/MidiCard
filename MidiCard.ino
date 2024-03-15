#include <Control_Surface.h>
USBMIDI_Interface midi;

//Board Version 1.1

/*
  Logic for a MIDI keyboard powered by a Seeed Studio Xiao SAMD21 with 25 multiplexed buttons - pins 0-4 are input, pins 6-10 are output.
  18 keys, and functions for OCT-, OCT+, SUS, MOD, P, MF, and FF.
*/

void setup() {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  Serial.begin(1115200);
  Control_Surface.begin(); 
}

//keep track of last button state for debounce
uint8_t lastG0 = 0;
uint8_t lastGS0 = 0;
uint8_t lastA0 = 0;
uint8_t lastAS0 = 0;
uint8_t lastB0 = 0;
uint8_t lastC = 0;
uint8_t lastCS = 0;
uint8_t lastD = 0;
uint8_t lastDS = 0;
uint8_t lastE = 0;
uint8_t lastF = 0;
uint8_t lastFS = 0;
uint8_t lastG = 0;
uint8_t lastGS = 0;
uint8_t lastA = 0;
uint8_t lastAS = 0;
uint8_t lastB = 0;
uint8_t lastC2 = 0;
uint8_t velocity = 127;
uint8_t lastSus = 0;
uint8_t lastMod = 0;
int8_t octaveOffset = 0;
uint8_t lastOctPl = 0;
uint8_t lastOctMi = 0;

//check given multiplex row for input, send note on/off
void checkMidi(uint8_t pin, uint8_t &lastState, uint8_t noteNum) {
  if(digitalRead(pin) == LOW){
    if(lastState == 0)
    {
      midi.sendNoteOn(noteNum + octaveOffset, velocity); 
      lastState = 1;
    }
  }else
  {
    if(lastState)
    {
      midi.sendNoteOff(noteNum + octaveOffset, 0); 
      lastState = 0;
    }
  }
}

void allNotesOff() {
  for(int i = 0; i < 127; i++)
  {
    midi.sendNoteOff(i, 0);
  }
}

void loop() {
   midi.update();
  
  //check column 1
  digitalWrite(6, LOW);
  checkMidi(0, lastG0, 55);
  checkMidi(1, lastC, 60);
  checkMidi(2, lastF, 65);
  checkMidi(3, lastAS, 70);

  if(digitalRead(4) == LOW){ //sustain
    if(lastSus == 0){
      midi.sendControlChange(64, 127);
      lastSus = 1;
    }
  }else{
    if(lastSus == 1)
    {
      midi.sendControlChange(64, 0);
      lastSus = 0;
    }
  }
  
  digitalWrite(6, HIGH);
  
  //check column 2
  digitalWrite(7, LOW);
  checkMidi(0, lastGS0, 56);
  checkMidi(1, lastCS, 61);
  checkMidi(2, lastFS, 66);
  checkMidi(3, lastB, 71);
  if(digitalRead(4) == LOW){ //modulation
    if(lastMod == 0){
      midi.sendControlChange(1, 127);
      lastMod = 1;
    }
  }else{
    if(lastMod == 1)
    {
      midi.sendControlChange(1, 0);
      lastMod = 0;
    }
  }
  
  digitalWrite(7, HIGH);
  
  //check column 3
  digitalWrite(8, LOW);
  checkMidi(0, lastA0, 57);
  checkMidi(1, lastD, 62);
  checkMidi(2, lastG, 67);
  checkMidi(3, lastC2, 72);
  if(digitalRead(4) == LOW){ //p button (quiet)
    velocity = 32;
  }
  digitalWrite(8, HIGH);
  
  //check column 4
  digitalWrite(9, LOW);
  checkMidi(0, lastAS0, 58);
  checkMidi(1, lastDS, 63);
  checkMidi(2, lastGS, 68);
  if(digitalRead(3) == LOW){ //octave down
  if(lastOctPl == 0)
    {
      lastOctMi = 0;
      lastOctPl = 1;
      if(octaveOffset < 36)
      {
        allNotesOff();
        octaveOffset += 12;
      }
    }
  }else
  {
    lastOctPl = 0;
  }
  
  if(digitalRead(4) == LOW){ //mf button (medium loud)
    velocity = 64;
  }
  
  digitalWrite(9, HIGH);
  
  //check column 5
  digitalWrite(10, LOW);
  checkMidi(0, lastB0, 59);
  checkMidi(1, lastE, 64);
  checkMidi(2, lastA, 69);
  if(digitalRead(3) == LOW){ //octave up  
    if(lastOctMi == 0)
    {
      lastOctMi = 1;
      lastOctPl = 0;
      if(octaveOffset > -36)
      {
        allNotesOff();
        octaveOffset -= 12;
      }
    }
  }else
  {
    lastOctMi = 0;
  }
  
  if(digitalRead(4) == LOW){ //ff button (as loud as possible)
    velocity = 127;
  }
  
  digitalWrite(10, HIGH);
  delay(10); //delay 10ms, refresh rate is plenty fast
}
