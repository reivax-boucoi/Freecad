#include "SplitFlapModule.h"

const char* ssid = "Freebox-6156D2";
const char* password = "b32r9q69r5qxx67kqt5746";

#define HALL_in     12  //GPIO12, conected to 74HC597 QH output (pin 9)
#define CLK_out     13  //GPIO13, connected to 74HC597 CLK and SCK (pins 12,11) and MIC59P60 CLK (pins 3)
#define STROBE_out  4   //GPIO4, connected to 74HC597 SLoad (pin 13, active low) and MIC59P60 STROBE (pins 8, latch when high)
#define MOTOR_out   0   // GPIO0, connected to 1st MIC59P60 DIN (pin 4)

#define NMODULE     6
#define IDLE_TIME 1000

uint8_t halls = 0;
uint32_t motors = 0xFFFFFF; // <<MSB=OUT8 chip near ESP (left most display). >>LSB OUT1 edge chip (right most display)
uint64_t pTime;

void shiftOUTData(uint32 d) {
  digitalWrite(STROBE_out, LOW);
  for (uint8_t i = 0; i < NMODULE * 4; i++) {

    digitalWrite(MOTOR_out, (d >> i) & 0x000001);
    digitalWrite(CLK_out, HIGH);
    digitalWrite(CLK_out, LOW);
  }
  digitalWrite(STROBE_out, HIGH);
}

uint8_t shiftINData() { // returns hall effect sensor data (LSB=right most display, 6th bit=left most display
  uint8_t res = 0;
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(CLK_out, HIGH);
    digitalWrite(CLK_out, LOW);
    if (digitalRead(HALL_in)) {
      res |= (1 << i);
    }
  }
  return res - 192;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HALL_in, INPUT);
  pinMode(CLK_out, OUTPUT);
  pinMode(STROBE_out, OUTPUT);
  pinMode(MOTOR_out, OUTPUT);
  digitalWrite(CLK_out, LOW);
  digitalWrite(STROBE_out, LOW);
  digitalWrite(MOTOR_out, LOW);

  shiftOUTData(motors);
  halls = shiftINData();
  pTime = millis();
}

void loop() {
  if (millis() > (pTime + IDLE_TIME)) {

    Serial.print(motors,BIN);
    if (motors > 0x00000) {
      motors = 0;
    } else {
      motors = 0xFFFFFF; //motors<<1;;
    }
    shiftOUTData(motors);
    halls = shiftINData();
    Serial.print("\t\t");
    Serial.println(halls,BIN);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));


    pTime = millis();
  }
}
