
#include <Arduino.h>

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

}

void loop() {
// go through drum matrix
  for (int column=1; column<17; column++) { // temporal. start with first beat point...
    for (int row=1; row<noOfDrumSteps; row++) { // vertical, outputs. start with output 0...

      // will the program run for this column?
      randValueSubtract = random(0, 5000);
      if (randValueSubtract > 1000) {  // if (randValueSubtract > analogRead(2)) {
        
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
        if (randValueAdd < 1000) { // if (randValueAdd < analogRead(3)) {
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
    delay(200); // delay after each beat == bpm
  }
}
