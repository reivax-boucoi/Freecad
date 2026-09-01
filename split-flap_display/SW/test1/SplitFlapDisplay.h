// SplitFlapDisplay.h (header file)
#ifndef SplitFlapDisplay_h
#define SplitFlapDisplay_h

#include <Arduino.h>
#include "SplitFlapModule.h"

#define NUM_MODULES 6
#define MAX_RPM 15.0f

class SplitFlapDisplay {
  public:

    SplitFlapDisplay(); //Constructor
    void init();
    void writeString(String inputString,float speed=MAX_RPM,bool centering=true); //Move all modules at once to show a specific string
    void writeChar(char inputChar,float speed = MAX_RPM); //sets all modules to a single char
    void moveTo(int targetPositions[], float speed = MAX_RPM, bool releaseMotors = true);
    void home(float speed = MAX_RPM); //move home
    void homeToString(String homeString,float speed = MAX_RPM,bool centering = true); //moves home and then writes a string
    void homeToChar(char homeChar,float speed = MAX_RPM); //moves home and then sets all modules to a char
    void testAll();
    void testCount();
    void testRandom(float speed = MAX_RPM);
    
  private:
    bool checkAllFalse(bool array[], int size);
    void stopMotors();
    void startMotors();

    SplitFlapModule modules[NUM_MODULES];   // Array of SplitFlapModule objects, size of array is maximum number of modules allowed in class
    static const int moduleOffsets[NUM_MODULES];
    static const uint8_t moduleAddresses[NUM_MODULES];

    static const float maxVel; //Max Velocity In RPM
    float maxStepsPerSecond; //calculated from maxVel
    static const int stepsPerRotation; //number of motor steps per full rotation of character drum
    static const int magnetPosition; //position of drum wheel when magnet is detected
  
};

#endif