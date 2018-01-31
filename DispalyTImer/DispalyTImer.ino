/*
   Author: Drew Applegath
   Updated: 01/31/2018

   Michigan Technological University - Undergraduate research
   Dr. Ong - Chem. Eng. Dept. 

   Displays the values for the input and runs the timer
*/

#include <EEPROM.h>
#include <LiquidCrystal.h>

// define inputs for buttons
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 11



int desiredSpinSpeed = 60; 
int desiredTiltSpeed = 0;  // start with tilt off
int mode = 0; 
unsigned long lastInputTime = 0; 

// declare LCD Device
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  pinMode(_MODE_, INPUT);
  pinMode(_PLUS_, INPUT); 
  pinMode(_MINUS_, INPUT); 

  // setup display for output/clear it
  lcd.begin(16, 2); 
  lcd.print("Initializing..."); 

  /* TODO: 
   * Read from EEPROM the values stored for speeds
   */
}

void loop() {
  // put your main code here, to run repeatedly:
  pollingLoop(); 
  if( ((millis() - lastInputTime) > 5) && (lastInputTime != 0) ) {
    lastInputTime = 0; 
    mode = 0; 
    storeValues(); 
  }

  updateDisplay(); 
  /*
   * Display the current mode 
   *  Mode 0 is the running values
   */
}

/*
 *  Poll the buttons and control the LCD display
 */
void pollingLoop() {
  // listen for mode button 
  if(digitalRead(_MODE_) == 1) {
    lastInputTime = millis(); // set last input
    if(mode > 5) {
      mode = 0; 
    }
    mode += 1; 
  }

  // listen for increase button 
  if(digitalRead(_PLUS_) == 1) {
    lastInputTime = millis(); // set last input
    switch(mode) {
      case 1: // set spin speed 
        desiredSpinSpeed += 1; 
        if (desiredSpinSpeed >= 120) {
          desiredSpinSpeed = 120; 
        }
        break;
        
      case 2: 
        break; 
      case 3: 
        break; 
      case 4: 
        break;   
    }
  }

  // listen for decrease button 
  if(digitalRead(_MINUS_) == 1) {
    lastInputTime = millis(); // set last input
    switch(mode) {
      case 1: // set spin speed
        desiredSpinSpeed -= 1; 
        if(desiredSpinSpeed < 0) {
          desiredSpinSpeed = 0; 
        }
        break; 
      case 2: 
        break; 
    }
  }
}

/*
 * Write the values to the EEPROM
 */
void storeValues() {
  // TODO Store all three values to memory. 
}

/*
 * Update the display to the current mode that is being used 
 */
void updateDisplay() {
  switch(mode) {
    case 0: 
      lcd.clear(); 
      lcd.print("RPM:"); 
      lcd.setCursor(5, 0);
      lcd.print(desiredSpinSpeed);
      lcd.setCursor(0,1);
      lcd.print("DEG:"); 
      lcd.setCursor(5,1); 
      lcd.print("N/A."); 
      //lcd.print((char)223);
      break; 
    case 1: 
      break; 
  }
}

