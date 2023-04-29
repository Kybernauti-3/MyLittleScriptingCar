
//-------------------------------------------------------------

#include "Arduino.h"
#include "EightStepMotor.h"

/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
EightStepMotor::EightStepMotor(int revolution, int pin_1, int pin_2, int pin_3, int pin_4)
{
  this->steps_left = 0;           // which step the motor is on
  this->clockwise = 0;            // motor direction
  this->running = 0;              // running state for the tick function
  this->last_step_time = 0;       // time stamp in us of the last step taken
  this->revolution = revolution;  // total number of steps for this motor
  
  // Values the motor pins can be feeded to:
  this->out_1 = 0b00000111;
  this->out_2 = 0b00011100;
  this->out_3 = 0b01110000;
  this->out_4 = 0b11000001;

  // Arduino pins for the motor control connection:
  this->pin_1 = pin_1;
  this->pin_2 = pin_2;
  this->pin_3 = pin_3;
  this->pin_4 = pin_4;

  // setup the pins on the microcontroller:
  pinMode(this->pin_1, OUTPUT);
  pinMode(this->pin_2, OUTPUT);
  pinMode(this->pin_3, OUTPUT);
  pinMode(this->pin_4, OUTPUT);
}

/*
 * Sets the speed in revolutions per minute *10
 * Calculates the time to wait between pulses.
 */
void EightStepMotor::setRPM(short rpm)
{
  if (rpm > 0) {
    this->running = 1;
    this->timer_us = 600L * 1000L * 1000L / this->revolution / rpm;
    this->clockwise = false;
  } else if (rpm <0) {
    this->running = 1;
    this->timer_us = 600L * 1000L * 1000L / this->revolution / -rpm;
    this->clockwise = true;
  } else {
    this->running = 0;
  }
}

/* 
 * Sets the steps to move the motor steps_count steps
 * If negative, the motor moves continuously.
 */
void EightStepMotor::setSteps(long degrees)
{
  this->steps_left = degrees * this->revolution / 360L;  // how many steps to take
}

/*
 * Function that takes care of moving the motor in the right time
 * Should be called in every loop of your main program.
 * Returns true when steps_left reaches 0.
 */
bool EightStepMotor::tick()
{
  if ((this->running) && (this->steps_left != 0))
  {
    unsigned long now = micros();
    if ((now - this->last_step_time) > this->timer_us)
    {
      this->last_step_time = now;
      if (this->steps_left > 0) { this->steps_left--; }
      this->stepMotor(this->clockwise);   // Moving the motor by one step
    }
    if (this->steps_left == 0) return true;
  }
  else  
  {
    digitalWrite(this->pin_1, 0);   // Release all pins to prevent sending current 
    digitalWrite(this->pin_2, 0);   // through the same coils and prevent overheating
    digitalWrite(this->pin_3, 0);
    digitalWrite(this->pin_4, 0);
  }
  return false;
}

/*
 * Returns number of steps that are left
 */
long EightStepMotor::getSteps()
{
  return this->steps_left * 360L / this->revolution;
}
/*
 * Returns real RPM *10 of the motor
 */
short EightStepMotor::getRPM() {
  if (!this->running)
    return 0;
  else if (!this->clockwise)
    return 600L * 1000L * 1000L / this->revolution / this->timer_us;
  else
    return -1* (short)(600L * 1000L * 1000L / this->revolution / this->timer_us);
}

/*
 * Rotates the motor clockwise or counterclockwise
 */
void EightStepMotor::stepMotor(bool clockwise)
{
  if (clockwise) {  // Making the step...
    this->out_1 = (this->out_1 << 1) | (this->out_1 >> 7);
    this->out_2 = (this->out_2 << 1) | (this->out_2 >> 7);
    this->out_3 = (this->out_3 << 1) | (this->out_3 >> 7);
    this->out_4 = (this->out_4 << 1) | (this->out_4 >> 7);
  } else {          // counterclockwise
    this->out_1 = (this->out_1 >> 1) | (this->out_1 << 7);
    this->out_2 = (this->out_2 >> 1) | (this->out_2 << 7);
    this->out_3 = (this->out_3 >> 1) | (this->out_3 << 7);
    this->out_4 = (this->out_4 >> 1) | (this->out_4 << 7);
  }
  // ... and writing the step to the pins:
  digitalWrite(this->pin_1, (this->out_1 & 1));
  digitalWrite(this->pin_2, (this->out_2 & 1));
  digitalWrite(this->pin_3, (this->out_3 & 1));
  digitalWrite(this->pin_4, (this->out_4 & 1));
}
