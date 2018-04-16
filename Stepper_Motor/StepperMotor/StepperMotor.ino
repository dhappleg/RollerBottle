#include <Stepper.h>

#define _HALF_STEP_ 8 

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(_HALF_STEP_, 8, 9, 10, 11);

void setup() {
  //myStepper.setMaxSpeed(1000); 
  //myStepper.setAcceleration(100); 
  myStepper.setSpeed(2000); 
  //steppMotor.run(); 
  // nothing to do inside the setup
}

void loop() {
  myStepper.step(1000); 
 // 
  
  // read the sensor value:
  //int sensorReading = analogRead(A0);
  // map it to a range from 0 to 100:

  
 // int motorSpeed = 50; //map(sensorReading, 0, 1023, 0, 100);
  // set the motor speed:
 // if (motorSpeed > 0) {
   // myStepper.setSpeed(100);
    // step 1/100 of a revolution:
   // myStepper.step(stepsPerRevolution/64);
 // }
}
