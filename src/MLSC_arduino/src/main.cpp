#include <Arduino.h>
#include <EightStepMotor.h>
#include <NewPing.h>

#define PINCOUNT 4
#define STEPS 8
#define REVOLUTION 4096

#define TRIGGER_PIN  6   // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     7   // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

float rpm = -15;
int timer_us = 60*1000L*1000L/REVOLUTION/rpm;
int lpins[PINCOUNT] = {8, 9, 10, 11};
int rpins[PINCOUNT] = {2, 3,  4,  5};

unsigned long last;
char s_mode;  // For handling the Serial input
char device;  // For handling the Serial input

bool ping_on = 0;
unsigned int last_dist = 0;  // Variable for saving the ultrasonic measurements
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

EightStepMotor LMotor(REVOLUTION, lpins[3], lpins[2], lpins[1], lpins[0]);
EightStepMotor RMotor(REVOLUTION, rpins[0], rpins[1], rpins[2], rpins[3]);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  if (sonar.check_timer()) { // Checks to see if the ping was received.
    last_dist = (sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
  }
}

void doSerial() {
  if (Serial.available() >= 7) {  // Beacause of that it is needed to be written as: gu00000
    s_mode = Serial.read();
    if (s_mode != 'g' && s_mode != 's') return;
    device = Serial.read();
    switch (device) {
    case 'r':
      if ((char)Serial.read() == 'v') {
        if (s_mode == 'g') Serial.println(rpm);
        else {
          if ((char)Serial.read() == '-') rpm = -((char)Serial.read()-'0')*10-((char)Serial.read()-'0')-(float)((char)Serial.read()-'0')/10;
          else rpm = ((char)Serial.read()-'0')*10+((char)Serial.read()-'0')+(float)((char)Serial.read()-'0')/10;
          RMotor.setRPM(rpm);
        }
      }
      else if ((char)Serial.read() == 'n') {
        if (s_mode == 'g') Serial.println(RMotor.getSteps());
        else RMotor.setSteps(((char)Serial.read()-'0')*1000+((char)Serial.read()-'0')*100+((char)Serial.read()-'0')*10+((char)Serial.read()-'0'));
      }
      break;
    case 'l':
      if ((char)Serial.read() == 'v') {
        if (s_mode == 'g') Serial.println(rpm);
        else {
          if ((char)Serial.read() == '-') rpm = -((char)Serial.read()-'0')*10-((char)Serial.read()-'0')-(float)((char)Serial.read()-'0')/10;
          else rpm = ((char)Serial.read()-'0')*10+((char)Serial.read()-'0')+(float)((char)Serial.read()-'0')/10;
          LMotor.setRPM(rpm);
        }
      }
      else if ((char)Serial.read() == 'n') {
        if (s_mode == 'g') Serial.println(LMotor.getSteps());
        else LMotor.setSteps(((char)Serial.read()-'0')*1000+((char)Serial.read()-'0')*100+((char)Serial.read()-'0')*10+((char)Serial.read()-'0'));
      }
      break;
    case 'u':
      if (s_mode == 'g') Serial.println(last_dist);
      else ping_on = (char)Serial.read() != '0';
      return;
    default:
      return;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<PINCOUNT; i++) {
    pinMode(lpins[i], OUTPUT);
  }
  for (int i=0; i<PINCOUNT; i++) {
    pinMode(rpins[i], OUTPUT);
  }
  last = millis();
  pingTimer = millis(); // Start now.
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  doSerial();

  LMotor.tick();
  RMotor.tick();
  if (!LMotor.getSteps()) { LMotor.setRPM(0); }   // This protects the motor from overheating
  if (!RMotor.getSteps()) { RMotor.setRPM(0); }   // This protects the motor from overheating

  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if ((millis() >= pingTimer) && ping_on) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
}
