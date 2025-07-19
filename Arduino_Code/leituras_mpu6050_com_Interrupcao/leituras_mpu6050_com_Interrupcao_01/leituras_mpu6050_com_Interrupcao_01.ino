#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>

#define MPU 0x68

#define xOffsetCalibration 155.53
#define yOffsetCalibration 235.69
#define zOffsetCalibration  1489.56

double ace[3]; //x, y, z
volatile bool leitura = false;

ISR(TIMER1_OVF_vect) {
	leitura = true;
  
  TCNT1 = 5535;
}


int main() {
  cli();

  Serial.begin(115200); 
  
  // Wire.begin();
  
  // Wire.beginTransmission(MPU);
  // Wire.write(0x6B);
  // Wire.write(0x00000000);
  // Wire.endTransmission(true);

  // Wire.beginTransmission(MPU);
  // Wire.write(0x1C);
  // Wire.write(0x00);
  // Wire.endTransmission(true);

  // Wire.beginTransmission(MPU);
  // Wire.write(0x1B);
  // Wire.write(0x00);
  // Wire.endTransmission(true);
  
  TCCR1B = (1<<CS12) | (0<<CS11)| (1<<CS10);
  TIMSK1 = 1<<TOIE1;
  TCNT1 = 5535; //top = 2499 para 0,01s
  
  sei();

  while(1)  { 
    if (leitura) {
   	  leitura = false;
      Serial.println("Aqui!");
    }
  }
}