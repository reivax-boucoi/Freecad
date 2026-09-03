#pragma once

#define HALL_in     12
#define CLK_out     13
#define STROBE_out  4
#define MOTOR_out   0

#define NMODULE             6
#define NFLAPS              37
#define ZERO_OFFSET_STEPS   23

#define HALF_STEP           1
#if HALF_STEP
  #define PHASE_COUNT       8
  #define STEPS_PER_REV     4096
#else
  #define PHASE_COUNT       4
  #define STEPS_PER_REV     2048
#endif

#define STEPS_PER_FLAP      ((float)STEPS_PER_REV / NFLAPS)

extern const char wheelChars[NFLAPS];

class DisplayModule {
public:

    DisplayModule(bool invert = false);

    void home();
    void gotoPos(uint8_t flap);
    void update(bool hall);

    uint8_t getOutputNibble() const;

private:

    bool invertDir;

    int32_t motorPos = 0;
    int32_t targetPos = 0;
    uint8_t phase = 0;

    bool enabled = false;
    bool homing = false;
    bool offsetting = false;
    uint16_t offsetCount = 0;
    bool hallPrev = false;

    void step();
    uint8_t currentFlap() const;
};

class DisplayController {
public:

    static constexpr uint16_t STEPPER_SPEED = 500; // steps/s

    DisplayController();

    void begin();
    void update();

    void home();
    void write(const char *txt, bool center = false);

    static volatile bool stepTick;

private:

    DisplayModule modules[NMODULE];

    uint8_t halls = 0;
    uint32_t motors = 0xFFFFFF;

    void shiftOUTData(uint32_t d);
    uint8_t shiftINData();

    uint8_t charToFlap(char c);
};

extern DisplayController display;