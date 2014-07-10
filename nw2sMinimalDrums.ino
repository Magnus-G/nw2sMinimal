#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// ----------------------------------------------------------------------------
// max4822.h
//
// Provides an SPI based interface to the MCP4822 dual voltage output digital 
// to analog to converter.
//
// Author: Will Dickson, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#ifndef _MCP4822_H_
#define _MCP4822_H_

#define MCP4822_NUMCHAN 2

enum MCP4822_DAC_CHAN {MCP4822_DAC_A, MCP4822_DAC_B};

class MCP4822 {
private:
    int cs;
    int ldac;
    int gain[MCP4822_NUMCHAN];
    int getCmdWrd(int dac, int value);
public:
    MCP4822();
    MCP4822(int csPin, int ldacPin);
    void begin();
    void setValue(int dac, int value);
    void setValue_A(int value);
    void setValue_B(int value);
    void setValue_AB(int value_A, int value_B);
    void setGain2X(int dac);
    void setGain2X_A();
    void setGain2X_B();
    void setGain2X_AB();
    void setGain1X(int dac);
    void setGain1X_A();
    void setGain1X_B();
    void setGain1X_AB();
    void off_AB();
    void off_A();
    void off_B();
    void off(int dac);
};
#endif


///////////////


// drums [z][y][x]
int const noOfDrumPrograms = 1;
int const noOfDrumSteps = 7;
int const noOfDrumOutputs = 6;
int drums[noOfDrumPrograms][noOfDrumOutputs][16] = {

  {
  // 1  2  3  4  5  6  7  8   1  2  3  4  5  6  7  8
    {1, 0, 0, 1, 0, 0, 1, 0,  0, 1, 0, 0, 1, 0, 0, 1}, // syncopation
    {1, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 1, 0, 1, 0}, // tom
    {0, 1, 0, 1, 0, 1, 0, 0,  0, 1, 0, 1, 0, 1, 0, 1}, // hihat
    {0, 0, 1, 1, 0, 0, 1, 0,  0, 0, 1, 1, 0, 0, 1, 0}, // snare
    {1, 0, 0, 0, 1, 0, 0, 0,  1, 0, 0, 0, 1, 0, 0, 0}, // bass
  } 
};

////////////////////////////////////////////////////////////

int pinOffset = 22;

// randomization/probability
int randValueSubtract = 1023;
int randValueAdd = 1023;

////////////////////////////////////////////////////////////

// for the on/off switch on clock in
int inputIsHigh = 0;
int digState[2] = {LOW, LOW};  // start with both set low

////////////////////////////////////////////////////////////

// counting drumloops. see drums.h for all drum patterns
int drumLoops = 0;
int drumDivision[6] = {1, 2, 4, 8, 16, 32};    

int sixth = 0;
int seventh = 0;
int eighth = 0;

////////////////////////////////////////////////////////////

int isThisATrigger[noOfDrumSteps] = {1,1,1,1,1,1,1};
int everyOtherTrigger[noOfDrumSteps] = {1,1,1,1,1,1,1};

////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  // setup Digital Out
  for (int i=22; i<38; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);

  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(42, OUTPUT);

  pinMode(50, OUTPUT);

}

////////////////////////////////////////////////////////////

void loop() {  

  analogWrite(12, 0);
  analogWrite(13, 100);
  analogWrite(14, 2000);

  analogWrite(40, 0);
  analogWrite(41, 100);
  analogWrite(42, 2000);

  analogWrite(50, 200);

// go through drum matrix
  for (int column=1; column<17; column++) { // temporal. start with first beat point...
    for (int row=1; row<noOfDrumSteps; row++) { // vertical, outputs. start with output 0...

      // will the program run for this column?
      randValueSubtract = random(0, 500);
      int randAnalogInSubtract = 500 - (constrain(analogRead(2), 0, 500));
      if (randValueSubtract > randAnalogInSubtract) {
        
        // set drumProgram
        int drumProgram = (analogRead(0) / (1023/noOfDrumPrograms)); if (drumProgram > 0) {  drumProgram--; } // deal with zero indexing on addressing the array vs the integer declared to set the number.
        
        // the hit
        if (drums[drumProgram][row-1][column-1] == 1) {
          digitalWrite(pinOffset+(row-1), 1);
        }

        // the 1 or 0 from the pattern is added to isThisATrigger
        isThisATrigger[row-1] = drums[drumProgram][row-1][column-1]; // give isThisATrigger a 1 or 0 depending on hit or not

        // a 1 is added to isThisATrigger anyway... maybe
        randValueAdd = random(0, 5000);
        int randAnalogInAdd = 500 - (constrain(analogRead(4), 0, 500));
        if (randValueAdd < randAnalogInAdd) {
          digitalWrite(pinOffset+(row-1), 1);
          isThisATrigger[row-1] = 1;
        }
        
        // check if gate should be turned off or kept open
        if (isThisATrigger[row-1] == 1) { // if this is a hit
          if (everyOtherTrigger[row-1] == 1) { // if the indicator variable shows 1
            digitalWrite(pinOffset+(row-1), LOW);  // turn gate off
            digitalWrite(pinOffset+(row-1), HIGH);  // turn gate on 
            digitalWrite(pinOffset+(row-1), LOW);  // turn gate off
            everyOtherTrigger[row-1] = 0; // indicator value set to 0 to indicate that the last hit turned gate off... so next one should keep it on and not go through this loop
          }
          else {
            everyOtherTrigger[row-1] = 1; // so that next time there will be a turning off 
          }
        }         
      } // rand subtract
    }
    int bpmSubtraction = 400 - (constrain(analogRead(0), 0, 399));
    delay(400 - bpmSubtraction); // delay after each beat == bpm
  }
}
