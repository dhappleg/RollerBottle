/*
   Author: Drew Applegath
   Updated: 01/19/2018

   Undergraduate research

   Computes RMP from hall effect sensor
*/

#include <EEPROM.h>
#include <LiquidCrystal.h>

volatile byte halfRev;
unsigned int rpm;
unsigned long timeOld;
int lightOn = 0;
int motorOn = 0;
int desiredSpeed = 0; 
bool speedChanging = false; 
//Timer t; // timer used for the saving of values to EEPROM
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);
  attachInterrupt(1, magnetInterrupt, FALLING); // interrupt to pin 2
  pinMode(8, OUTPUT);
  pinMode(9, INPUT); 
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  desiredSpeed = EEPROM.read(0); 
  //EEPROM.write(0,69); 
  //EEPROM.write(1, 1); 
}

void loop() {
  // Speed calculations HERE:
  //Serial.println(digitalRead(9)); 
  //delay(100); 
  if(digitalRead(9) == 1) {
    desiredSpeed++; 
    updateSpeed(); 
  }
  else {
    if(digitalRead(10) == 1) {
      desiredSpeed--; 
      updateSpeed();
    }
  }
  //t.update();
  /* if (halfRev >= 20) {
      rpm = 30*1000/(millis() - timeOld)*halfRev;
      timeOld = millis();
      halfRev = 0;
      Serial.println(rpm,DEC);
    }*/
}

void storeValues() {
  EEPROM.write(0, (uint8_t)desiredSpeed); 
}

void updateSpeed() {
 // t.after(1000, storeValues); 
  speedChanging = true; 
  lcd.clear(); 
  lcd.print("Desired RPM"); 
  lcd.setCursor(0,1); 
  lcd.print((String) desiredSpeed); 
  delay(300); 
  speedChanging = false;
}

void magnetInterrupt () {
  halfRev ++;
  Serial.println("rise edge:");
  toggleLight();
  //toggleMotor();
}

void writeDisplay(String val) {
  if(!speedChanging) {
    lcd.clear(); 
    lcd.print("RPM:"); 
    lcd.setCursor(5, 0);
    lcd.print(val);
    lcd.setCursor(0,1);
    lcd.print("DEG:"); 
    lcd.setCursor(5,1); 
    lcd.print("N/A."); 
    lcd.print((char)223);
  }
}

void toggleLight() {
  if (lightOn) {
    digitalWrite(8, LOW);
    writeDisplay((String) EEPROM.read(0));
    lightOn = 0;
  } else {
    digitalWrite(8, HIGH);
    writeDisplay((String) EEPROM.read(1));
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

