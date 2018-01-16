/*
 * Author: Drew Applegath
 * Updated: 01/15/2018
 * 
 * Undergraduate research
 * 
 * Computes RMP from hall effect sensor
 */

#include <LiquidCrystal.h>

volatile byte halfRev; 
unsigned int rpm; 
unsigned long timeOld; 
int lightOn = 0; 
int motorOn = 0; 
LiquidCrystal lcd(1, 2, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);
  attachInterrupt(1, magnet_interrupt, FALLING); // interrupt to pin 2
  pinMode(8, OUTPUT);

}

void loop() {
  // Speed calculations HERE:  
 /* if (halfRev >= 20) { 
     rpm = 30*1000/(millis() - timeOld)*halfRev;
     timeOld = millis();
     halfRev = 0;
     Serial.println(rpm,DEC);
   }*/
}

void magnet_interrupt () {
  halfRev ++; 
  Serial.println("rise edge:");
  toggleLight(); 
  //toggleMotor(); 
}

void toggleLight() {
  if(lightOn) {
    digitalWrite(8, LOW); 
    lightOn = 0; 
  } else {
    digitalWrite(8, HIGH); 
    lightOn = 1; 
  }
}

void toggleMotor() {
  if(motorOn) {
    digitalWrite(3, HIGH); 
    motorOn = 0; 
  } else {
    digitalWrite(3, LOW); 
    motorOn = 1; 
  }
}

