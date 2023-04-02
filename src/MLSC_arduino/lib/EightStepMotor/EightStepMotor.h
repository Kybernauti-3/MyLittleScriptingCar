
// ensure this library description is only included once
#ifndef EightStepMotor_h
#define EightStepMotor_h

// library interface description
class EightStepMotor {
  public:
    // constructor:
    EightStepMotor(int revolution, int pin_1, int pin_2, int pin_3, int pin_4);

    // method for setting the speed:
    void setRPM(long rpm);

    // method for setting the steps to move the motor:
    void setSteps(long steps_count);

    // Main loop that takes care of moving the motor
    void tick();

    // method for getting left steps:
    long getSteps();

  private:
    void stepMotor(bool clockwise);

    bool clockwise;           // Direction of rotation
    bool running;             // Determines, wether the motor is running, or not
    unsigned long timer_us;   // delay between steps, in us, based on speed
    int revolution;           // total number of steps this motor can take
    long steps_left;           // how many steps are left, if negative, go continuously, if 0, stop

    // pin states (values):
    byte out_1;
    byte out_2;
    byte out_3;
    byte out_4;

    // motor pin numbers:
    int pin_1;
    int pin_2;
    int pin_3;
    int pin_4;

    unsigned long last_step_time; // time stamp in us of when the last step was taken
    // TODO: Use timer interrupts instead
};

#endif

