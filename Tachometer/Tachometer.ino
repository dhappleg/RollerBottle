int ledOn = 0; 

void setup() {
  Serial.begin(115200); 
  pinMode(13, OUTPUT); 
  attachInterrupt(0, irInterruput, FALLING); 
}

void loop() {
  delay(1000); 
}

void irInterruput() {
  (ledOn == 1) ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW); 
  (ledOn == 1) ? ledOn = 0 : ledOn = 1;  
}

