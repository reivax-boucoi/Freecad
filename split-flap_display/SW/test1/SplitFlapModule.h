#pragma once

#include <Arduino.h>
#include <Wire.h>

#define NUMCHARS 37

class SplitFlapModule {
  public:
    SplitFlapModule();
    SplitFlapModule(int stepsPerFullRotation, int stepOffset, int magnetPos);

    void init();
    void writeIO(uint8_t step);
    void step(bool updatePosition = true);                   // step motor
    void stop();                                             // write all motor input pins to low
    void start();                                            // re-energize coils to last position, not stepping motor

    int getMagnetPosition() const { return magnetPosition; } // position where magnet is detected
    int getCharPosition(char inputChar);                     // get integer position given single character
    int getPosition() const { return position; }             // get integer position

    bool readHallEffectSensor();                             // return the value read by the hall effect
    // sensor
    void magnetDetected() {
        position = magnetPosition;
    } // update position to magnetposition, called when magnet is detected

    bool getHasErrored() const { return hasErrored; }

  private:
    int position;                   // character drum position
    int stepNumber;                 // current position in the stepping order, to make motor move
    int stepsPerRot;                // number of steps per rotation
    bool hasErrored = false;        // flag to indicate if an error has occurred

    void writeIO(uint16_t data);    // write to motor in pins

    int magnetPosition;             // altered by offsets
    static const int motorPins[];   // Array of motor pins
    static const int HallEffectPIN; // Hall Effect Sensor Pin

    int charPositions[NUMCHARS];

    static const char StandardChars[NUMCHARS];
    static const uint8_t stepperSequence[8];
};
