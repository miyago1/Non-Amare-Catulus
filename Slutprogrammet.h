#define threshold_tid 1000        // I millisekunder
#define threshold_ljud  150       // 0 och 1023 baserat på 0 och 5 volt från ADCn
#define threshold_snark  3
#define LONG_DELAY_MS 900000      // 900000 ms = 15 minuter



// Varning globala variabler!
int choice = 0;                              // Väckningsval. Används av ISR (PCINT2_vect), ISR (PCINT0_vect) och vackningsMetod()
volatile int toggleADC = 0;                  // ADCn på eller av. Används av ISR (PCINT0_vect) och start_ADC()
int counter_snark = 0;                       // Räknar anatalet långvariga ljud. Används av vackningsMetod() och start_ADC()

// Funktionsdeklarering

int ljudTid ();                   // Mäter tiden på ett ljud

void vackningsMetod();            // Väckningsmetoder
void startaVibration();           
void startaLjud();
void startaMotor();

void pciSetup(byte pin);          // Initierar interrupts på pins

void start_ADC();                 // Lyssnar efter ljud

void val_knapp(int knappval);


// interrupt service routine
ISR (PCINT2_vect) {                           // Interrupt för väckningsval knappar
          if (digitalRead(7))  {              // Ljud valt
            val_knapp(1); choice = 1;
        } else if (digitalRead(6)) {          // Vibration valt
            val_knapp(2); choice = 2;
        } else if (digitalRead(5)) {             // Täckesryckning valt
            val_knapp(3); choice = 3;
     }
}
ISR (PCINT0_vect)                             // interrupt för startar ADC knapp
 {    
           if (digitalRead(8)) {               // Startar ADC
            if (toggleADC == 0 && choice != 0) {
            toggleADC = 1;
            start_ADC();
            } else {
            digitalWrite(13, LOW);
            toggleADC = 0;
            interrupts();  
            }
        }
 }


// Arduinos stantard setup() och loop()
void setup() {
    
ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // ADC Prescaler 128

      // Lampor
      pinMode(13, OUTPUT);                            // ADC lampa          
      pinMode(12, OUTPUT);                            // Ljud lampa             
      pinMode(11, OUTPUT);                            // Vibrations lampa           
      pinMode(10, OUTPUT);                            // Motor lampa
      
      // Väckningar
      
      pinMode(4, OUTPUT);                             // Vibration larm
      pinMode(3, OUTPUT);                             // Ljudväckning
      pinMode(2, OUTPUT);                             // Täckeslarm
      
     
    // Förbered interrupt för digital utgångar
      pciSetup(8);                                    // ADC knapp          PCINT0
      pciSetup(7);                                    // Ljud knapp         PCINT23
      pciSetup(6);                                    // Vibration knapp    PCINT22
      pciSetup(5);                                    // Motor knapp        PCINT21
    }
    
void loop() {
      }
// slut på arduinos standard


// Funktionsdefinitioner
int ljudTid () {              // Räknar hur lång tid ljud är över gränsen
  int counter = 0;
  do {
    counter = counter + 10;
    delay(10);
  } while (analogRead(A0) > threshold_ljud);
  return counter;             // Tiden i millisekunder
}
void vackningsMetod(){
  counter_snark = 0;            // Nollställer snarkningsräknaren
  switch (choice) {
                    case 1 : startaLjud(); break;               // startar ljudlarm
                    case 2 : startaVibration(); break;          // startar vibration
                    case 3 : startaMotor(); break;              // Startar motorn
                  }
}
void startaVibration(){
  digitalWrite(2, HIGH);            // Vibrerar i 3 sekunder
  delay(3000); 
  digitalWrite(2, LOW);
  delay(3 * 1000);                 // Tid att ändra position
}
void startaLjud(){
  analogWrite(3, 2);                // 3 alarm 1 sekund långa
  delay(1000); 
  analogWrite(3, 0); 
  delay(1000); 
  analogWrite(3, 2); 
  delay(1000); 
  analogWrite(3, 0); 
  delay(1000); 
  analogWrite(3, 2); 
  delay(1000); 
  analogWrite(3, 0);
  delay(3 * 1000);                 // Tid att ändra position
}
void startaMotor(){
  digitalWrite(4, HIGH); 
  delay(5000); 
  digitalWrite(4, LOW);
  delay(3 * 1000);                 // Tid att ändra position
}
void pciSetup(byte pin){
        *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));   // Pekar bit
        PCIFR  |= bit (digitalPinToPCICRbit(pin));                    // Cleara interrupt flaggan
        PCICR  |= bit (digitalPinToPCICRbit(pin));                    // Startar interrupt
}



void start_ADC() {
        digitalWrite(13, HIGH);                       // Tänd lampan för ADCn
        interrupts();                                 // Enablar interrupts igen noInterrupts ();
        PCMSK2 &= bit(PCINT21);                       // Stänger av interrupts på d 6 och 5 och 7
        PCMSK2 &= bit(PCINT22);
        PCMSK2 &= bit(PCINT23);
        delay(LONG_DELAY_MS);                         // Ge användaren tid att somna
        int counter_tid = 0;
       
        do {
            if (analogRead(A0) > threshold_ljud) {                         // Tröskel?
                counter_tid = ljudTid();
                if (counter_tid > threshold_tid) {
                  counter_snark++;
                }
                if (counter_snark > threshold_snark){
                  vackningsMetod();
                }
            }
        } while(toggleADC == 1);
        PCMSK2 |= bit(PCINT21);       // interrupt på igen 
        PCMSK2 |= bit(PCINT22);
        PCMSK2 |= bit(PCINT23);
    }
    
void val_knapp(int knappval) {
  switch(knappval) {
        case 1 : digitalWrite(12, HIGH); digitalWrite(11, LOW); digitalWrite(10, LOW); break;
        case 2 : digitalWrite(11, HIGH); digitalWrite(12, LOW); digitalWrite(10, LOW); break;
        case 3 : digitalWrite(10, HIGH); digitalWrite(11, LOW); digitalWrite(12, LOW); break;
}
            
}