#include <Ticker.h>
#include "DisplayController.h"

Ticker stepTimer;

void IRAM_ATTR stepISR() {
    DisplayController::stepTick = true;
}

void setup() {

    Serial.begin(115200);

    display.begin();

    stepTimer.attach_us(1000000UL / DisplayController::STEPPER_SPEED,stepISR);

    display.home();

    // display.write("HELLO", true);
}

void loop() {

    if (DisplayController::stepTick) {

        DisplayController::stepTick = false;
        display.update();
    }
}