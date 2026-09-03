#include <Ticker.h>
#include "DisplayController.h"

Ticker stepTimer;
volatile bool stepFlag = false;

uint32_t pTime;
uint8_t pos=10;

void IRAM_ATTR stepISR() {
    stepFlag = true;
}

void setup() {

    Serial.begin(115200);
    Serial.println("Restarted");
    display.begin();

    stepTimer.attach_ms(1000.0 / STEPPER_SPEED,stepISR);

    //display.home();

    // display.write("HELLO", true);
    pTime=millis();
}

void loop() {

    if (stepFlag) {
        stepFlag = false;
        display.update();
    }

    if(millis()>(pTime+5000)){
        Serial.print("Goto ");
        Serial.println(pos);
        display.modules[0].gotoPos(pos);
        pos=10-pos;
        pTime=millis();
    }
}
