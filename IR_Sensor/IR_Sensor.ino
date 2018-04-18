#include <LiquidCrystal.h>

#define _HALL_SENSOR_ 0   // on interrupt 1 pin 3
#define _MODE_  9
#define _PLUS_  10
#define _MINUS_ 11
#define _MAX_SPIN_SPEED_ 120

// used for blinking the LED on pin 13
int ledOn = 0;  
volatile byte rpmCount;
unsigned int rpm = 0; 
int mode = 0;
int oldDispTime = 0; 
unsigned long lastInputTime = 0; 
int previousPlus = 0; 
int previousMinus = 0;
int desiredSpinSpeed = 60;  
int mappedRPM = 6; 
unsigned long timeOld; 
int loopCounter = 0; 


// declare LCD Device
LiquidCrystal lcd(13, 12, 4, 5, 6, 7);

/*
 * used for the stepper motor
 */
void enableTimer() {
  noInterrupts(); 
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  interrupts(); 
}

ISR(TIMER0_COMPA_vect) { 
  digitalWrite(8, HIGH); 
  digitalWrite(8, LOW); 
}

void setup() {
  Serial.begin(115200);
  
  // setup display for output/clear it
  lcd.begin(16, 2); 
  lcd.print("Initializing...");
  pinMode(8, OUTPUT); 
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(_MODE_, INPUT);
  pinMode(_PLUS_, INPUT); 
  pinMode(_MINUS_, INPUT);
  enableTimer(); 
  attachInterrupt(_HALL_SENSOR_, irInterrupt, FALLING);
  rpm = 0; 
  timeOld = 0; 

  delay(1000); 
}

void loop() { 
  rpmCalculation();
  pollingLoop(); 
  if( ((millis() - lastInputTime) > 5000) && (lastInputTime != 0) ) {
    lastInputTime = 0; 
    mode = 0;  
  }
  if(oldDispTime > 2000) {
    oldDispTime = 0;   
    update_display();
  } 

  //if(loopCounter > 9) {
    //loopCounter = 0;
    //digitalWrite(8, HIGH); 
   // delayMicroseconds(10); 
  //delayMicroseconds(90); 
   // digitalWrite(8, LOW); 
  //}
  /*mappedRPM = desiredSpinSpeed;

  if(loopCounter <= 1) {
    digitalWrite(8, HIGH); 
  } else {
    if(loopCounter > mappedRPM) {
      digitalWrite(8, LOW);
      loopCounter = 0; 
    }
  } 
  loopCounter++; */
  //delayMicroseconds(90); 
  loopCounter++; 
  oldDispTime++; 
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
 *  Poll the buttons and control the LCD display
 */
void pollingLoop() {
  // listen for mode button 
  if( (millis() - lastInputTime) > 470) {
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
  //delay(470); 
}

