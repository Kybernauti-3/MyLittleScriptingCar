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

// Variables for Serial communication:
String command = ""; 

// Variables for ultrasonic sensor:
unsigned long last;
bool ping_on = 0;
unsigned int last_dist = 0;  // Variable for saving the ultrasonic measurements
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

EightStepMotor *m;
EightStepMotor LMotor(REVOLUTION, lpins[3], lpins[2], lpins[1], lpins[0]);
EightStepMotor RMotor(REVOLUTION, rpins[3], rpins[2], rpins[1], rpins[0]);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  if (sonar.check_timer()) { // Checks to see if the ping was received.
    last_dist = (sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
  }
}

void doCommand(String command) {
  // determine the device we want to use:
  switch (command[1])
  {
  case 'r':
    m = &RMotor;
    break;
  
  case 'l':
    m = &LMotor;
    break;
  
  case 'u':  // Whole code for ultrasonic sensor:
    if (command[0] == 'g') {
      Serial.print("u");
      Serial.println(last_dist);
    }
    else if (command[0] == 's') { ping_on = command[2]-'0'; }
    return;
    
  default:
    return;
  }

  // Code for controling/reading motor values:
  if (command[0] == 'g') {
    if (command[2] == 'v') {
      Serial.print(command[1]);
      Serial.print("v");
      Serial.println(m->getRPM());
    }
    else if (command[2] == 'n') {
      Serial.print(command[1]);
      Serial.print("n");
      Serial.println(m->getSteps());
    }
  }
  else if (command[0] == 's') {
    if (command[2] == 'v') { m->setRPM(command.substring(3).toInt()); }
    else if (command[2] == 'n') { m->setSteps(command.substring(3).toInt()); }
  }
}

void doSerial() {
  if (Serial.available() > 0) {
    char incomingChar = Serial.read();
    
    // If the string is complete, run this:
    if (incomingChar == '\n') {
      Serial.print("Received string: ");
      Serial.println(command);
      doCommand(command);
      command = "";
      return;
    }
    
    // Add the character to the input string
    command += incomingChar; 
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

  command.reserve(64);
  command = "";

  last = millis();
  pingTimer = millis(); // Start now.
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  doSerial();

  if (LMotor.tick()) Serial.println("ln0");
  if (RMotor.tick()) Serial.println("rn0");

  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if ((millis() >= pingTimer) && ping_on) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
}
