#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>

#define MPU 0x68

volatile bool read = false;

ISR(TIMER2_OVF_vect) {
  read = true;

  TCNT2 = 99;
}

void setup() {

  Serial.begin(115200);

  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
  TIMSK2 = 1<<TOIE2;
  TCNT2 = 99;

  

  sei();
}

void loop() {
    if (read) {
      read = false;
      Serial.println("Aqui!");
    }
}
