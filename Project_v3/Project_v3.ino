/*
 * Author: Drew Applegath
 * Project: Undergraduate Research - Roller Bottle
 */
#include <LiquidCrystal.h>

#define _STEP_ 8
#define _IR_SENSOR_ 0   // on interrupt 0 pin 2
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 11

#define _MAX_SPIN_SPEED_ 120

// declare LCD Device
LiquidCrystal lcd(13, 12, 4, 5, 6, 7); 

// declare global variables 
unsigned int rpm, rpmCount = 0; // speed calculations
unsigned long lastInputTime = 0; // used to keep button presses to a min
int previousPlus = 0; 
int previousMinus = 0;
int desiredSpinSpeed = 60; 
int mode = 0; // used for display
int loopCounter = 0; // used for delay
unsigned long timeOld; 
int oldDispTime = 0; 

/*
 * Interrupt service routine for timer 1
 */
ISR(TIMER1_COMPA_vect) { 
  digitalWrite(_STEP_, HIGH); 
  digitalWrite(_STEP_, LOW); 
  OCR1A = 2*desiredSpinSpeed;  
}

/*
 * Interrupt service routine for ir sensor
 */
void irInterrupt() {
  //toggleOnBoardLED(); 
  rpmCount++; 
}

void rpmCalculation() {
  detachInterrupt(_IR_SENSOR_); 
  rpm = 30*1000/(millis() - timeOld) * rpmCount;
  timeOld = millis(); 
  rpmCount = 0;
  attachInterrupt(_IR_SENSOR_, irInterrupt, RISING);
}

void pollingLoop() {
  if( (millis() - lastInputTime) > 470) {
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
    }
  } else {
    previousMinus = 0; 
  }
  }
}

/*
 * Update the display to the current mode that is being used 
 */
void update_display() {
  lcd.clear(); 
  switch(mode) {
    case 0: // running/sensing mode
      lcd.print("RPM:"); 
      lcd.setCursor(5,0);
      lcd.print((String) rpm);
      /*lcd.setCursor(0,1);
      lcd.print("DEG:"); 
      lcd.setCursor(5,1); 
      lcd.print("--.-"); 
      lcd.print((char)223); // degree symbol
      */
      break;
   
    case 1: // update speed mode
      lcd.setCursor(1,0); 
      lcd.print("Desired Speed"); 
      lcd.setCursor(0,1); 
      lcd.print((String) desiredSpinSpeed);
      lcd.setCursor(5,1); 
      lcd.print("(RPM)"); 
      break; 
  } 
}

/*
 * 
 */
void timerSetup() {
  noInterrupts(); 
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 200;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) ;//| (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts(); 
}

/*
 * 
 */
void setup() {
  // set baud rate
  Serial.begin(115200); 

  // setup display for output/clear it
  lcd.begin(16, 2); 
  lcd.print("Initializing...");

  // declare output pins
  //pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(_STEP_, OUTPUT); 
  // setup buttons for polling unput
  pinMode(_MODE_, INPUT);
  pinMode(_PLUS_, INPUT); 
  pinMode(_MINUS_, INPUT);

  // enable and setup interrupts on timers and tachometer
  timerSetup();
  attachInterrupt(_IR_SENSOR_, irInterrupt, FALLING); 

  // for visual effect
  delay(1000); 
}

/*
 * Loop the program, used for polling the buttons
 */
void loop() { 
  rpmCalculation();
  pollingLoop();
  if( ((millis() - lastInputTime) > 5000) && (lastInputTime != 0) ) {
    lastInputTime = 0; 
    mode = 0;  
  }
  if(oldDispTime > 7500) {
    oldDispTime = 0;   
    update_display();
  } 
  loopCounter++; 
  oldDispTime++; 
}

/*
 * Used for interrupt debugging
 */
void toggleOnBoardLED() {
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1); 
}
