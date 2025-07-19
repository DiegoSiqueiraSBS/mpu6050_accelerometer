#include <avr/io.h> 
#include <avr/interrupt.h>
#include <Wire.h>

const int MPU = 0x68;
float ace[3];
float aceAnt[3] = {0, 0, 0};

float T = 0.01;

float gyroX, gyroY, gyroZ;

float vel[3];
float velAnt[3] = {0, 0, 0};

float dist[3];
float distAnt[3] = {0, 0, 0};

float xOffsetCalibration = 155.53;
float yOffsetCalibration = 235.69;
float zOffsetCalibration = 1489.56;

void leitura() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  ace[0] = (((Wire.read() << 8 | Wire.read()) - xOffsetCalibration) / 16401.43) * 9.8;
  ace[1] = (((Wire.read() << 8 | Wire.read()) - yOffsetCalibration) / 16455.37) * 9.8;
  ace[2] = (((Wire.read() << 8 | Wire.read()) - zOffsetCalibration) / 16744.62) * 9.8;

  // Wire.beginTransmission(MPU);
  // Wire.write(0x43);
  // Wire.endTransmission(false);

  // Wire.requestFrom(MPU, 6, true);

  // gyroX = ((Wire.read() << 8 | Wire.read()) / 131) + 4;
  // gyroY = (Wire.read() << 8 | Wire.read()) / 131;
  // gyroZ = (Wire.read() << 8 | Wire.read()) / 131;

}

ISR(TIMER1_OVF_vect){ //interrupção do TC1

  //vel = last_vel + (last_acel + acel) * t / 2.0;

  Serial.println("Aqui!");
  leitura();
  vel[0] = velAnt[0] + (aceAnt[0] + ace[0]) * T / 2.0;
  dist[0] = distAnt[0] + (velAnt[0] + vel[0]) * T / 2.0;

  Serial.print(10);
  Serial.print(",");
  Serial.print(ace[0]);
  Serial.print(",");
  Serial.print(vel[0]);
  Serial.print(",");
  Serial.print(dist[0]);
  Serial.print(-10);
  Serial.println();

  TCNT1 = 63036;
} 

//---------------------------------------------------------------------------------- 
int main() { 
  init();
  Serial.begin(115200); 

  Wire.begin();

  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00000000);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  TCCR1B = (0<<CS12) | (1<<CS11)| (1<<CS10);//TC0 com prescaler de 64, a 16 MHz gera uma 
  TIMSK1 = 1<<TOIE1; //habilita a interrupção do TC1
  TCNT1 = 63036; //top

 sei(); //habilita a chave de interrupção global 

 while(1) { 
  
 }
}