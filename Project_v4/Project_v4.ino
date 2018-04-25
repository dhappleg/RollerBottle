/*
 * Author: Drew Applegath
 * Project: Undergraduate Research - Roller Bottle
 */
 
#include <LiquidCrystal.h>

#define _IR_SENSOR_ 0
#define _STEP_ 8
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 11

#define _MAX_SPEED_ 120

//LiquidCrystal lcd(9, 12, 4, 5, 6, 7); 
LiquidCrystal lcd(13, 12, 4, 5, 6, 7); 

int mode, previousPlus, previousMinus = 0; 
int desiredSpeed = 60; 
unsigned int rpm, rpmCount, loopCount = 0; 
unsigned long timeOld, lastInput = 0; 

void setup() {
  lcd.begin(16, 2); 
  lcd.print("Initializing...");
  pinMode(_STEP_, OUTPUT); 
  //pinMode(_MODE_, INPUT);
  //pinMode(_PLUS_, INPUT); 
  //pinMode(_MINUS_, INPUT);
    
  attachInterrupt(_IR_SENSOR_, ir_ISR, RISING); 
  init_timer(); 
  
  delay(1000); 
}

void loop() {
  poll_input(); 
  if( ((millis() - lastInput) > 5000) && (lastInput != 0) ) {
    lastInput = 0; 
    mode = 0;  
  }
  // calculate every second
  if( ((millis() - timeOld) > 3000) && rpmCount >= 1) {
    calculate_rpm(); 
  }
  if(loopCount > 14000) {
    loopCount = 0; 
    //calculate_rpm();
    update_display();
  } 
  //delay(470); 
  loopCount++; 
}

/*
 * Interrupt service routine for timer 1
 */
ISR(TIMER1_COMPA_vect) { 
  digitalWrite(_STEP_, HIGH); 
  digitalWrite(_STEP_, LOW); 
  //OCR1A = 2*desiredSpeed;  
  //OCR1A = desiredSpeed;
  OCR1A = 624; 
}

void ir_ISR() {
  rpmCount++; 
}

/*
 * 
 */
void init_timer() {
  //cli(); 
  noInterrupts(); 
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (0 << WGM11); // set 16 bit
  TCCR1B |= (0 << WGM10); // set 16 bit
  // Set CS10 and CS12 bits for 1024 prescaler
 // TCCR1B |= (1 << CS12) ;//| (1 << CS10); 
  TCCR1B |= B00000010;  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //attachInterrupt(_IR_SENSOR_, ir_ISR, RISING); 
  //sei(); 
  interrupts();
}

void calculate_rpm() {
  noInterrupts(); 
  rpm = (((60/4)*1000)/(millis() - timeOld))* rpmCount ;//( ((rpmCount/8)/(millis() - timeOld) )/60000);//30 * 1000 / (millis() - timeOld) * rpmCount;
  //rpm = rpmCount; 
  timeOld = millis(); 
  rpmCount = 0; 
  attachInterrupt(_IR_SENSOR_, ir_ISR, RISING); 
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
      lcd.print((String) rpm);
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
