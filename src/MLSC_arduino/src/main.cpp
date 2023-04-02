#include <Arduino.h>
#include <EightStepMotor.h>

#define PINCOUNT 4
#define STEPS 8
#define REVOLUTION 4096

int rpm = 15;
int timer_us = 60*1000L*1000L/REVOLUTION/rpm;
int pins[PINCOUNT] = {8, 9, 10, 11};

unsigned long last;

bool clockwise[STEPS][PINCOUNT] = {
  {0, 0, 0, 1},
  {0, 0, 1, 1},
  {0, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {1, 0, 0, 1}
};

EightStepMotor Motor(REVOLUTION, pins[0], pins[1], pins[2], pins[3]);

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<PINCOUNT; i++) {
    pinMode(pins[i], OUTPUT);
  }
  last = millis();
  Serial.begin(9600);
  Motor.setRPM(rpm);
  Motor.setSteps(5L*REVOLUTION);
  Serial.println(Motor.getSteps());
}

void loop() {
  // put your main code here, to run repeatedly:
  Motor.tick();
  if (!Motor.getSteps()) { Motor.setRPM(0); }  // This protects the motor from overheating
}
