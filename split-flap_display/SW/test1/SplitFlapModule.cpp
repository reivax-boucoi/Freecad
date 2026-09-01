#include "SplitFlapModule.h"


const char SplitFlapModule::StandardChars[NUMCHARS] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                                                       'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
                                                       'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
                                                      };
bool hasErrored = false;
const uint8_t SplitFlapModule::stepperSequence[8] = {0b0111, 0b0011, 0b1011, 0b1001, 0b1101, 0b1100, 0b1110, 0b0110};

SplitFlapModule::SplitFlapModule() : position(0), stepNumber(0), stepsPerRot(0) {
  magnetPosition = 710;
}

SplitFlapModule::SplitFlapModule(int stepsPerFullRotation, int stepOffset, int magnetPos): position(0), stepNumber(0), stepsPerRot(stepsPerFullRotation) {
  magnetPosition = magnetPos + stepOffset;
}

void SplitFlapModule::init() {
  float stepSize = (float) stepsPerRot / (float) NUMCHARS;
  float currentPosition = 0;
  for (int i = 0; i < NUMCHARS; i++) {
    charPositions[i] = (int) currentPosition;
    currentPosition += stepSize;
  }

  //uint16_t initState = 0b1111111111100001; // Pin 15 (17) as INPUT, Pins 1-4 as OUTPUT
  writeIO((uint8_t)0x0F);
  stop();                                  // Write all motor coil inputs LOW

  for (uint8_t i = 0; i < 8; i++) {
    step();
    delay(100);
  }
  stop();
}

int SplitFlapModule::getCharPosition(char inputChar) {
  inputChar = toupper(inputChar);
  for (int i = 0; i < NUMCHARS; i++) {
    if (StandardChars[i] == inputChar) {
      return charPositions[i];
    }
  }
  return 0; // Character not found, return blank
}

void SplitFlapModule::stop() {
  writeIO((uint8_t)0x0F);
}

void SplitFlapModule::start() {
  stepNumber = (stepNumber + 7) % 8; // effectively take one off stepNumber
  step(false);                       // write the "previous" step high again, in case turned off
}

void SplitFlapModule::step(bool updatePosition) {
  writeIO(stepperSequence[stepNumber]);
  if (updatePosition) {
    position = (position + 1) % stepsPerRot;
    stepNumber = (stepNumber + 1) % 8;
  }
}

bool SplitFlapModule::readHallEffectSensor() {
  if (hasErrored) {
    return false;
  }

  bool state = true; //TODO

  return state;
}

void writeIO(uint8_t step) {
  return;

}
