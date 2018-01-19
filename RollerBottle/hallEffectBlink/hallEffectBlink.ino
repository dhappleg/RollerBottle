/*
   Author: Drew Applegath
   Updated: 01/15/2018

   Undergraduate research

   Computes RMP from hall effect sensor
*/

#include <LiquidCrystal.h>

volatile byte halfRev;
unsigned int rpm;
unsigned long timeOld;
int lightOn = 0;
int motorOn = 0;
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);
  attachInterrupt(1, magnetInterrupt, FALLING); // interrupt to pin 2
  pinMode(8, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Rounds Per Min.");

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

void magnetInterrupt () {
  halfRev ++;
  Serial.println("rise edge:");
  toggleLight();
  //toggleMotor();
}

void writeDisplay(String val) {
  lcd.setCursor(0, 1);
  lcd.print(val);
}

void toggleLight() {
  if (lightOn) {
    digitalWrite(8, LOW);
    writeDisplay("148.2");
    lightOn = 0;
  } else {
    digitalWrite(8, HIGH);
    writeDisplay("99.98");
    lightOn = 1;
  }
}

void toggleMotor() {
  if (motorOn) {
    digitalWrite(3, HIGH);
    motorOn = 0;
  } else {
    digitalWrite(3, LOW);
    motorOn = 1;
  }
}

