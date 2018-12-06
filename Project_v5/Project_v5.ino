/*
 * Author: Drew Applegath
 * Project: Undergraduate Research - Roller Bottle
 * Updated: 12/05/2018
 */
 
// Add libraries for EEPROM and LCD Display 
#include <LiquidCrystal.h>
#include <EEPROM.h>

// define globaly used variable for pins and setting 
// speed values. 
#define _STEP_ 8
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 11
#define _MAX_SPEED_ 120

// Declare lcd display and pinout
LiquidCrystal lcd(13, 12, 4, 5, 6, 7); 

/*
 * initialize and declare variables
 * 
 * mode          - [Functionality removed] Tracks current operating mode
 *                  for lcd user input. 
 * previousPlus  - Stores user input for "plus" button
 * previousMinus - Stores user input for "minus" button
 * loopCount     - Used for non delayed timing calculations (clk counter)
 * lastInput     - Tracks user input for +/- 10 operation
 * desiredSpeed  - Default desired speed to 1 rpm for new devices
 */
int mode, previousPlus, previousMinus = 0; 
unsigned int loopCount = 0; 
unsigned long lastInput = 0; 
int desiredSpeed = 60;

/*
 * Define and initialize values and timers.
 */
void setup() {
  // setup and start the lcd display
  lcd.begin(16, 2); 
  lcd.print("Initializing...");

  // declare inputs and outputs for buttons and stepper motor
  pinMode(_STEP_, OUTPUT); 
  pinMode(_MODE_, INPUT);
  pinMode(_PLUS_, INPUT); 
  pinMode(_MINUS_, INPUT);

  // get data from EEPROM, mod 120 to make sure other values 
  // stored from previous uses arent accidently passed to 
  // speed calculation
  desiredSpeed = (EEPROM.read(0)) % 120;  

  // call to set timer values for stepper control  
  init_timer(); 
  
  // delay for user effect
  delay(1000); 
}

/* 
 * Iteratively check the inputs for change and update 
 * desired speed depending on the input. 
 */
void loop() {
  // check the buttons for user input
  poll_input(); 

  // Calculate display update and look for new input
  if( ((millis() - lastInput) > 5000) && (lastInput != 0) ) {
    lastInput = 0; 
    mode = 0;  
    EEPROM.write(0, desiredSpeed); 
    // check for zero case and do not listen on timer interrupt
    if(desiredSpeed <=0) {
      noInterrupts();
    } else {
      interrupts(); 
    }
  }

  // update the display every second
  if(loopCount > 14000) {
    loopCount = 0; 
    update_display();
  } 
  // update non delay time values
  loopCount++; 
}

/*
 * Interrupt service routine for timer 1
 * 
 * Pulse a step to the motor and update the timer. 
 */
ISR(TIMER1_COMPA_vect) { 
  digitalWrite(_STEP_, HIGH); 
  digitalWrite(_STEP_, LOW);   
  OCR1A = 4686 / desiredSpeed; // calculated step per rpm/desired rpm
}

/*
 * Sets the inrnal timer 1 to 16bit and the pre-scalor value to 1024.  
 */
void init_timer() {
  noInterrupts(); 
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 4686 / desiredSpeed; // calculated step per rpm/desired rpm
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (0 << WGM11); // set 16 bit
  TCCR1B |= (0 << WGM10); // set 16 bit
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10); 
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A); 
  interrupts();
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
      lcd.print((String) desiredSpeed); 
      break;
    case 1: // update speed mode
      lcd.setCursor(1,0); 
      lcd.print("Desired Speed"); 
      lcd.setCursor(0,1); 
      lcd.print((String) desiredSpeed);
      lcd.setCursor(5,1); 
      lcd.print("(RPM)"); 
      break; 
  } 
}

/* 
 * Check for input pins to have a high value and determine if the
 * button press meets the requirements for incrememting by 10 after
 * 5 depressed values in a row. 
 */
void poll_input() {
  if( (millis() - lastInput) > 470) {
  // listen for mode button
  if(digitalRead(_MODE_) == 1) {
    lastInput = millis(); // set last input
    if(mode >= 1) {
      mode = 0; 
    }
    mode += 1; 
  }

  // listen for increase button 
  if(digitalRead(_PLUS_) == 1) {
    previousPlus++; 
    lastInput = millis(); // set last input
    switch(mode) {
      case 1: // increase the desired spin speed 
        if(previousPlus >= 5) {
          desiredSpeed += 10; 
        } else {
           desiredSpeed += 1; 
        }
        if (desiredSpeed >= _MAX_SPEED_) {
          desiredSpeed = _MAX_SPEED_; 
        }
        break;  
    }
  } else {
    previousPlus = 0;
  }
  // listen for decrease button 
  if(digitalRead(_MINUS_) == 1) {
    previousMinus++; 
    lastInput = millis(); // set last input
    switch(mode) {
      case 1: // set spin speed
        if (previousMinus >= 5) {
          desiredSpeed -=10; 
        } else {
          desiredSpeed -= 1; 
        }
        if(desiredSpeed < 0) {
          desiredSpeed = 0; 
        }
        break;  
    }
  } else {
    previousMinus = 0; 
  }
  }
}
