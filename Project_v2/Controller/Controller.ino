/*
   Author: Drew Applegath
   Updated: 02/25/2018

   Michigan Technological University - Undergraduate research
   Dr. Ong - Chem. Eng. Dept. 

   Displays the values for the input and runs the timer
*/

#include <EEPROM.h>
#include <LiquidCrystal.h>

// define inputs for buttons
#define _HALL_SENSOR_ 1   // on interrupt 1 pin 3
#define _MOTOR_CONTORLLER_ 11
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 2

#define _SPIN_SPEED_MEM_ 0
#define _TILE_SPEED_MEM_ 1
#define _TILE_ANGLE_MEM_ 2 

#define _MAX_SPIN_SPEED_ 120
#define _MAX_TILT_SPEED_ 10
#define _MAX_TILT_ANGLE_ 10 



int desiredSpinSpeed = 60; 
int desiredTiltSpeed =  0;  // start with tilt off
int desiredTiltAngle =  0;  // no tilt 
int mode = 0;  
unsigned char motorSpeed = 0; 
unsigned long lastInputTime = 0; 
int previousPlus = 0; 
int previousMinus = 0; 

int lightOn = 0;

// declare LCD Device
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  attachInterrupt(_HALL_SENSOR_, magnet_interrupt, FALLING); 
  pinMode(_MODE_, INPUT);
  pinMode(_PLUS_, INPUT); 
  pinMode(_MINUS_, INPUT);
  pinMode(11, OUTPUT); 
  pinMode(3, OUTPUT); 

  // setup display for output/clear it
  lcd.begin(16, 2); 
  lcd.print("Initializing...");   

  /* TODO: 
   * Read from EEPROM the values stored for speeds
   */
  load_values(); 
   
  delay(1000);  // just for initialization effect.
}

void loop() {
  // put your main code here, to run repeatedly:
  polling_lop(); 
  if( ((millis() - lastInputTime) > 5000) && (lastInputTime != 0) ) {
    lastInputTime = 0; 
    mode = 0; 
    store_values(); 
  }

  update_display(); 
  update_motor(); 
  /*
   * Display the current mode 
   *  Mode 0 is the running values
   */
  analogWrite(3, 255); 
}

/*
 * update motor speed
 */
void update_motor() {
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS22);
  OCR2A = desiredTiltSpeed;
  
}

/*
 * Blink the light attached to pin 8. 
 */
void toggle_light() {
  if (lightOn) {
    digitalWrite(8, LOW);
   // writeDisplay((String) EEPROM.read(0));
    lightOn = 0;
  } else {
    digitalWrite(8, HIGH);
    //writeDisplay((String) EEPROM.read(1));
    lightOn = 1;
  }
}

/*
 * Speed calculation for rpm of wheels
 */
void magnet_interrupt() {
  toggle_light(); 
}

/*
 *  Poll the buttons and control the LCD display
 */
void polling_lop() {
  // listen for mode button 
  if(digitalRead(_MODE_) == 1) {
    lastInputTime = millis(); // set last input
    if(mode >= 1) {
      mode = 0; 
    }
    mode += 1; 
  }

  // listen for increase button 
  if(digitalRead(_PLUS_) == 1) {
    previousPlus++; 
    lastInputTime = millis(); // set last input
    switch(mode) {
      case 1: // increase the desired spin speed 
        if(previousPlus >= 5) {
          desiredSpinSpeed += 10; 
        } else {
           desiredSpinSpeed += 1; 
        }
        if (desiredSpinSpeed >= _MAX_SPIN_SPEED_) {
          desiredSpinSpeed = _MAX_SPIN_SPEED_; 
        }
        break;
        
//      case 2: // increasing the desired tilt speed
//        desiredTiltSpeed += 1; 
//        if(desiredTiltSpeed > _MAX_TILT_SPEED_) {
//          desiredTiltSpeed = _MAX_TILT_SPEED_;
//        }
//        break; 
//      case 3: // increasing the desired tilt angle
//        desiredTiltAngle += 1; 
//        if(desiredTiltAngle > _MAX_TILT_ANGLE_) {
//          desiredTiltAngle = _MAX_TILT_ANGLE_; 
//        }
//        break;   
    }
  } else {
    previousPlus = 0;
  }

  // listen for decrease button 
  if(digitalRead(_MINUS_) == 1) {
    previousMinus++; 
    lastInputTime = millis(); // set last input
    switch(mode) {
      case 1: // set spin speed
        if (previousMinus >= 5) {
          desiredSpinSpeed -=10; 
        } else {
          desiredSpinSpeed -= 1; 
        }
        if(desiredSpinSpeed < 0) {
          desiredSpinSpeed = 0; 
        }
        break; 
//      case 2: // decreasing the desired tilt speed
//        desiredTiltSpeed -=1; 
//        if(desiredTiltSpeed < 0) {
//          desiredTiltSpeed = 0; 
//        }
//        break; 
//
//      case 3: // decreasing the desired tilt angle
//        desiredTiltAngle -= 1; 
//        if(desiredTiltAngle < 0) {
//          desiredTiltAngle = 0; 
//        }
//        break; 
    }
  } else {
    previousMinus = 0; 
  }
}

/*
 * Write the values to the EEPROM for speed, angle, and tilt speed
 */
void store_values() {
  EEPROM.write(_SPIN_SPEED_MEM_, (uint8_t) desiredSpinSpeed);
  EEPROM.write(_TILE_SPEED_MEM_, (uint8_t) desiredTiltSpeed); 
  EEPROM.write(_TILE_ANGLE_MEM_, (uint8_t) desiredTiltAngle); 
}

/*
 * Read the values from the EEPROM for speed, angle, and tilt speed
 */
void load_values() {
  desiredSpinSpeed = EEPROM.read(_SPIN_SPEED_MEM_); 
  desiredTiltSpeed = EEPROM.read(_TILE_SPEED_MEM_); 
  desiredTiltAngle = EEPROM.read(_TILE_ANGLE_MEM_); 
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
      lcd.print("---");
      lcd.setCursor(0,1);
      lcd.print("DEG:"); 
      lcd.setCursor(5,1); 
      lcd.print("--.-"); 
      lcd.print((char)223); // degree symbol
      break;
   
    case 1: // update speed mode
      lcd.print("Desired RPM"); 
      lcd.setCursor(0,1); 
      lcd.print((String) desiredSpinSpeed);
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
  delay(270); 
}

