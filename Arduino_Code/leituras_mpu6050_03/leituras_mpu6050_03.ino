#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>

#define MPU 0x68

#define alpha 0.09

float t = 0.00;

int16_t rawAceX, rawAceY, rawAceZ;
float conAceX, conAceY, conAceZ;
float aceX, aceY, aceZ;
float antAceX, antAceY, antAceZ;

int16_t rawGyrX, rawGyrY, rawGyrZ;
float conGyrX, conGyrY, conGyrZ;
float gyrX, gyrY, gyrZ;
float antGyrX, antGyrY, antGyrZ;

volatile bool read = false;

void inicialization() {
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
}

void readSensor() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  // Dados Brutos Acelerômetro
  rawAceX = (Wire.read() << 8 | Wire.read());
  rawAceY = (Wire.read() << 8 | Wire.read());
  rawAceZ = (Wire.read() << 8 | Wire.read());

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  // Dados Brutos Giroscópio
  rawGyrX = (Wire.read() << 8 | Wire.read());
  rawGyrY = (Wire.read() << 8 | Wire.read());
  rawGyrZ = (Wire.read() << 8 | Wire.read());

  // Conversão em g
  conAceX = rawAceX / 16384.0;
  conAceY = rawAceY / 16384.0;
  conAceZ = rawAceZ / 16384.0;

  // Conversão em º/s
  conGyrX = rawGyrX / 131.0;
  conGyrY = rawGyrY / 131.0;
  conGyrZ = rawGyrZ / 131.0;

  // Filtro passa-baixa acelerômetro
  aceX = alpha * conAceX + (1 - alpha) * antAceX;
  aceY = alpha * conAceY + (1 - alpha) * antAceY;
  aceZ = alpha * conAceZ + (1 - alpha) * antAceZ;

  // Filtro passa-baixa giroscópio

  gyrX = alpha * conGyrX + (1 - alpha) * antGyrX;
  gyrY = alpha * conGyrY + (1 - alpha) * antGyrY;
  gyrZ = alpha * conGyrZ + (1 - alpha) * antGyrZ;

  antAceX = aceX;
  antAceY = aceY;
  antAceZ = aceZ;

  antGyrX = gyrX;
  antGyrY = gyrY;
  antGyrZ = gyrZ;

}

ISR(TIMER2_OVF_vect) {
  t = t + 0.01;
  read = true;

  TCNT2 = 99;
}

void setup() {

  Serial.begin(115200);
  inicialization();

  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
  TIMSK2 = 1<<TOIE2;
  TCNT2 = 99;

  sei();
}

void loop() {
    if (read) {
      read = false;
      readSensor();
      Serial.print(t);
      Serial.print(",");
      Serial.print(aceX);
      Serial.print(",");
      Serial.print(aceY);
      Serial.print(",");
      Serial.print(aceZ);
      Serial.println();
    }
}
