void setup() {
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // ADC Prescaler 128
  Serial.begin(9600);
}


void loop() {

  
  int values = 0;
  int result = 0;

  if (analogRead(A0) > 100) {
  for(int i=0;i<500;i++){
    values = values + analogRead(A0);
    delay(1);
  }
result = (values / 500);
  for(int i=0;i<500;i++){
    Serial.println(result);
    delay(1);
  }
  } else {
    for(int i=0;i<500;i++){
    Serial.println(result);
    }
  }
  

}