#include <LiquidCrystal.h>

#define _HALL_SENSOR_ 0   // on interrupt 1 pin 3

// used for blinking the LED on pin 13
int ledOn = 0;  
volatile byte rpmCount;
unsigned int rpm = 0; 
int mode = 0;
unsigned long timeOld; 

// declare LCD Device
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);
  
  // setup display for output/clear it
  lcd.begin(16, 2); 
  lcd.print("Initializing...");
  
  pinMode(LED_BUILTIN, OUTPUT); 
  attachInterrupt(_HALL_SENSOR_, irInterrupt, FALLING);
  rpm = 0; 
  timeOld = 0; 

  delay(1000); 
}

void loop() { 
  rpmCalculation();
  update_display(); 
}

void rpmCalculation() {
  detachInterrupt(_HALL_SENSOR_); 
  rpm = 30*1000/(millis() - timeOld) * rpmCount;
  timeOld = millis(); 
  rpmCount = 0;
  attachInterrupt(_HALL_SENSOR_, irInterrupt, RISING);
}

void irInterrupt() {
  //toggleOnBoardLED(); 
  rpmCount++; 
}

void toggleOnBoardLED() {
  (ledOn == 1) ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW); 
  (ledOn == 1) ? ledOn = 0 : ledOn = 1; 
}

/*
 * Update the display to the current mode that is being used 
 */
void update_display() {
  lcd.clear(); 
  switch(mode) {
    case 0: // running/sensing mode
      lcd.print("RPM:"); 
      lcd.setCursor(5, 0);
      lcd.print((String) rpm);
      lcd.setCursor(0,1);
      lcd.print("DEG:"); 
      lcd.setCursor(5,1); 
      lcd.print("--.-"); 
      lcd.print((char)223); // degree symbol
      break;
   
    case 1: // update speed mode
      lcd.print("Desired RPM"); 
      lcd.setCursor(0,1); 
      lcd.print((String) rpm);
      break; 
    
//    case 2: // update tilt speed mode
//      lcd.print("Desired Tilt CPM"); 
//      lcd.setCursor(0,1); 
//      lcd.print((String) desiredTiltSpeed); 
//      break; 
//   
//    case 3: // update tilt angle
//      lcd.print("Desired Tilt Angle"); 
//      lcd.setCursor(0,1);
//      lcd.print((String) desiredTiltAngle); 
//      lcd.print((char)223); // degree symbol
//      break; 
  } 
  delay(470); 
}

