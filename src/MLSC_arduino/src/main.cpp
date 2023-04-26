#include <Arduino.h>
#include <EightStepMotor.h>
#include <NewPing.h>

#define PINCOUNT 4
#define STEPS 8
#define REVOLUTION 4096

#define TRIGGER_PIN  7   // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     6   // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int rpm = -15;
int timer_us = 60*1000L*1000L/REVOLUTION/rpm;
int lpins[PINCOUNT] = {8, 9, 10, 11};
int rpins[PINCOUNT] = {2, 3,  4,  5};

unsigned long last;

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

EightStepMotor LMotor(REVOLUTION, lpins[3], lpins[2], lpins[1], lpins[0]);
EightStepMotor RMotor(REVOLUTION, rpins[0], rpins[1], rpins[2], rpins[3]);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
  }
  // Don't do anything here!
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
  LMotor.setRPM(rpm);
  RMotor.setRPM(rpm);
  LMotor.setSteps(5L*REVOLUTION);
  RMotor.setSteps(5L*REVOLUTION);
}

void loop() {
  // put your main code here, to run repeatedly:
  LMotor.tick();
  RMotor.tick();
  if (!LMotor.getSteps()) { LMotor.setRPM(0); }   // This protects the motor from overheating
  if (!RMotor.getSteps()) { RMotor.setRPM(0); }   // This protects the motor from overheating

  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
}
