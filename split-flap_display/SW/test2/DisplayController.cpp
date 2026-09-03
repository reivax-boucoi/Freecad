#include "DisplayController.h"

const char wheelChars[NFLAPS] = {
    ' ','A','B','C','D','E','F','G','H','I',
    'J','K','L','M','N','O','P','Q','R','S',
    'T','U','V','W','X','Y','Z',
    '0','1','2','3','4','5','6','7','8','9'
};

volatile bool DisplayController::stepTick = false;

#if HALF_STEP

static const uint8_t seq[8] = {
    0b0111,
    0b0011,
    0b1011,
    0b1001,
    0b1101,
    0b1100,
    0b1110,
    0b0110
};

#else

static const uint8_t seq[4] = {
    0b0111,
    0b1011,
    0b1101,
    0b1110
};

#endif

DisplayModule::DisplayModule(bool invert) :
    invertDir(invert) {
}

uint8_t DisplayModule::currentFlap() const {
    return ((uint32_t)round((float)motorPos / STEPS_PER_FLAP)) % NFLAPS;
}

void DisplayModule::home() {
    homing = true;
    offsetting = false;
    enabled = true;
}

void DisplayModule::gotoPos(uint8_t flap) {

    uint8_t delta =(flap + NFLAPS - currentFlap()) % NFLAPS;
    targetPos = motorPos + (int32_t)round(delta * STEPS_PER_FLAP);
    enabled = true;
}

void DisplayModule::step() {

    if (!enabled)
        enabled = true;

    phase = invertDir ?
        (phase + PHASE_COUNT - 1) % PHASE_COUNT :
        (phase + 1) % PHASE_COUNT;

    motorPos++;
}

void DisplayModule::update(bool hall) {

    bool rising = !hallPrev && hall;
    hallPrev = hall;

    if (homing) {

        step();

        if (rising) {
            homing = false;
            offsetting = true;
            offsetCount = ZERO_OFFSET_STEPS;
            motorPos = -ZERO_OFFSET_STEPS;
        }

        return;
    }

    if (offsetting) {

        step();

        if (--offsetCount == 0) {

            offsetting = false;
            motorPos = 0;
            targetPos = 0;
            enabled = false;
        }

        return;
    }

    if (rising) {

        offsetting = true;
        offsetCount = ZERO_OFFSET_STEPS;
        motorPos = -ZERO_OFFSET_STEPS;
    }

    if (motorPos < targetPos) {

        step();
    }
    else {

        enabled = false;
    }
}

uint8_t DisplayModule::getOutputNibble() const {
    if (!enabled)return 0x0F;
    return seq[phase];
}

// ---------------------------------------------------------------------------

DisplayController::DisplayController() :
    modules{
        DisplayModule(false),
        DisplayModule(false),
        DisplayModule(false),
        DisplayModule(false),
        DisplayModule(false),
        DisplayModule(false)
    } {
}

void DisplayController::begin() {

    pinMode(HALL_in, INPUT);
    pinMode(CLK_out, OUTPUT);
    pinMode(STROBE_out, OUTPUT);
    pinMode(MOTOR_out, OUTPUT);

    digitalWrite(CLK_out, LOW);
    digitalWrite(STROBE_out, LOW);
    digitalWrite(MOTOR_out, LOW);

    shiftOUTData(motors);
}

void DisplayController::home() {
    for (auto &m : modules)
        m.home();
}

uint8_t DisplayController::charToFlap(char c) {

    c = toupper(c);

    for (uint8_t i = 0; i < NFLAPS; i++) {
        if (wheelChars[i] == c)
            return i;
    }

    return 0;
}

void DisplayController::write(const char *txt, bool center) {

    char buf[NMODULE];
    memset(buf, ' ', NMODULE);
    uint8_t len = min((uint8_t)strlen(txt), (uint8_t)NMODULE);
    uint8_t start =center ? (NMODULE - len) / 2 : 0;
    memcpy(&buf[start], txt, len);
    for (uint8_t i = 0; i < NMODULE; i++)
        modules[i].gotoPos(charToFlap(buf[i]));
}

void DisplayController::update() {

    halls = shiftINData();
    for (uint8_t i = 0; i < NMODULE; i++)
        modules[i].update((halls >> i) & 1);

    motors = 0;
    for (uint8_t i = 0; i < NMODULE; i++)
        motors |= (uint32_t)modules[i].getOutputNibble() << (i * 4);
    shiftOUTData(motors);
}

void DisplayController::shiftOUTData(uint32_t d) {

    digitalWrite(STROBE_out, LOW);
    for (uint8_t i = 0; i < NMODULE * 4; i++) {

        digitalWrite(MOTOR_out, (d >> i) & 1);
        digitalWrite(CLK_out, HIGH);
        digitalWrite(CLK_out, LOW);
    }
    digitalWrite(STROBE_out, HIGH);
}

uint8_t DisplayController::shiftINData() {

    uint8_t res = 0;
    digitalWrite(STROBE_out, LOW);
    digitalWrite(STROBE_out, HIGH);
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(CLK_out, HIGH);
        digitalWrite(CLK_out, LOW);
        if (digitalRead(HALL_in))
            res |= (1 << i);
    }
    return res - 192;
}

DisplayController display;