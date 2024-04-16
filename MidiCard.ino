#include <Control_Surface.h>
USBMIDI_Interface midi;

//Board Version Plus V1.0

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
  pinMode(5, INPUT_PULLUP);
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
  //Serial.begin(1115200);
  Control_Surface.begin(); 
}

//keep track of last button state for debounce
uint8_t lastC0 = 0;
uint8_t lastCS0 = 0;
uint8_t lastD0 = 0;
uint8_t lastDS0 = 0;
uint8_t lastE0 = 0;
uint8_t lastF0 = 0;
uint8_t lastFS0 = 0;
uint8_t lastG0 = 0;
uint8_t lastGS0 = 0;
uint8_t lastA0 = 0;
uint8_t lastAS0 = 0;
uint8_t lastB0 = 0;
uint8_t lastC1 = 0;
uint8_t lastCS1 = 0;
uint8_t lastD1 = 0;
uint8_t lastDS1 = 0;
uint8_t lastE1 = 0;
uint8_t lastF1 = 0;
uint8_t lastFS1 = 0;
uint8_t lastG1 = 0;
uint8_t lastGS1 = 0;
uint8_t lastA1 = 0;
uint8_t lastAS1 = 0;
uint8_t lastB1 = 0;
uint8_t lastC2 = 0;
uint8_t velocity = 64;
uint8_t lastVelPressed = 0;
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
  //check C0, F0, A#0, D#1, G#1, OctMi
  checkMidi(0, lastC0, 48);
  checkMidi(1, lastF0, 53);
  checkMidi(2, lastAS0, 58);
  checkMidi(3, lastDS1, 63);
  checkMidi(4, lastGS1, 68);
  if (digitalRead(5) == LOW)
  { // octave down
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
  }
  else
  {
    lastOctMi = 0;
  }
  
  digitalWrite(6, HIGH);
  
  //check column 2
  digitalWrite(7, LOW);
  //check CS0, FS0, B0, E1, A1, OctPl
  checkMidi(0, lastCS0, 49);
  checkMidi(1, lastFS0, 54);
  checkMidi(2, lastB0, 59);
  checkMidi(3, lastE1, 64);
  checkMidi(4, lastA1, 69);
  if (digitalRead(5) == LOW)
  { // octave up
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
  }
  else
  {
    lastOctPl = 0;
  }

  digitalWrite(7, HIGH);
  
  //check column 3
  digitalWrite(8, LOW);
  //check D0, G0, C1, F1, AS1, Sus
  checkMidi(0, lastD0, 50);
  checkMidi(1, lastG0, 55);
  checkMidi(2, lastC1, 60);
  checkMidi(3, lastF1, 65);
  checkMidi(4, lastAS1, 70);
  //sus 5
  if (digitalRead(5) == LOW)
  { // sustain
    if (lastSus == 0)
    {
      midi.sendControlChange(64, 127);
      lastSus = 1;
    }
  }
  else
  {
    if (lastSus == 1)
    {
      midi.sendControlChange(64, 0);
      lastSus = 0;
    }
  }

  digitalWrite(8, HIGH);
  
  //check column 4
  digitalWrite(9, LOW);
  //check DS0, GS0, CS1, FS1, B1, Mod
  checkMidi(0, lastDS0, 51);
  checkMidi(1, lastGS0, 56);
  checkMidi(2, lastCS1, 61);
  checkMidi(3, lastFS1, 66);
  checkMidi(4, lastB1, 71);

  if (digitalRead(5) == LOW)
  { // modulation
    if (lastMod == 0)
    {
      midi.sendControlChange(1, 127);
      lastMod = 1;
    }
  }
  else
  {
    if (lastMod == 1)
    {
      midi.sendControlChange(1, 0);
      lastMod = 0;
    }
  }

  digitalWrite(9, HIGH);
  
  //check column 5
  digitalWrite(10, LOW);
  //check E0, A0, D1, G1, C2, Vel
  checkMidi(0, lastE0, 52);
  checkMidi(1, lastA0, 57);
  checkMidi(2, lastD1, 62);
  checkMidi(3, lastG1, 67);
  checkMidi(4, lastC2, 72);
  
  if(digitalRead(5) == LOW){ //toggle between three dynamics
    if(lastVelPressed == 0)
    {
      lastVelPressed = 1;
      //toggle between mf, ff, and p
      if(velocity == 127)
      {
        velocity = 64;
      }else if(velocity == 64)
      {
        velocity = 100;
      }else
      {
        velocity = 127;
      }
    }
  }else {
    if(lastVelPressed == 1){
      lastVelPressed = 0;
    }
  }
  
  digitalWrite(10, HIGH);
  delay(10); //delay 10ms, refresh rate is plenty fast
}
