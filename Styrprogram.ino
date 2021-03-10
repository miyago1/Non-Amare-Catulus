#include <LowPower.h>    


 

    void pciSetup(byte pin)
    {
        *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
        PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
        PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
    }

     
    // Use one Routine to handle each group
        int choice = 0;         // Väckningsval
        volatile int toggleADC = 0; // är ADCn av eller på
        int troskel = 600;        // Troskelvärdet #1

    void start_ADC() {
        
        digitalWrite(13, HIGH);
        interrupts();                    // Enablar interrupts igen noInterrupts ();
        PCMSK2 &= bit(PCINT21);         // Stänger av interrupts på d 6 och 5
        PCMSK2 &= bit(PCINT22);
       
        do {
            if (analogRead(A1) > troskel) {             // Tröskel?
                switch (choice) {
                    case 1 : digitalWrite(9, HIGH); break;    // startar ljudlarm
                    case 2 : digitalWrite(10, HIGH); break;    // startar vibration
                }
            }
        } while(toggleADC == 1);
        PCMSK2 |= bit(PCINT21);     // interrupt på igen 
        PCMSK2 |= bit(PCINT22);
    }
    
    
    void val_ljud() {
        digitalWrite(12, LOW);      // Släck
        digitalWrite(11, HIGH);     // indikator lampa tänd
        choice = 1;
    }
      
    void val_vibration() { 
        digitalWrite(11, LOW);    // Släck
        digitalWrite(12, HIGH);   // indikator lampa tänd
        choice = 2;
    }    
            
    ISR (PCINT2_vect) {                    // Interrupt för väckningsval
          if (digitalRead(6))  {         // Ljud valt
            val_ljud();
        } else if (digitalRead(5))           // Vibration valt
            val_vibration();


     }

ISR (PCINT0_vect) // interrupt för startar ADC knapp
 {    
           if (digitalRead(8)) {               // Startar ADC
            if (toggleADC == 0 && choice != 0) {
            toggleADC = 1;
            start_ADC();
            } else {
            digitalWrite(13,LOW);
            toggleADC = 0;
            interrupts();  
            }
        }
 } 

    void setup() {
Serial.begin(9600);
ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // ADC Prescaler 128
      pinMode(13, OUTPUT);      // ADC lampa          
      pinMode(12, OUTPUT);      // Vibrations lampa             
      pinMode(11, OUTPUT);       // Ljud lampa           
      pinMode(10, OUTPUT);      // Vibrations larm            
      pinMode(9, OUTPUT);       // Ljud larm           
      // pinMode(8, INPUT);                  
     
    // enable interrupt for pin...
      pciSetup(8);      // ADC
      pciSetup(6);      // Ljud
      pciSetup(5);      // Vibration
    }
     
     
    void loop() {
      }


