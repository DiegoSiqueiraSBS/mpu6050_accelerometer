#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>

#define MPU 0x68

#define alpha 0.09
#define alphaGyro 0.09

float rawAx, rawAy, rawAz;
float rawGx, rawGy, rawGz;

float ax, ay, az;
float gx, gy, gz;

float lowAx, lowAy, lowAz;
float lowGx, lowGy, lowGz;
float highAx, highAy, highAz;
float highx, highGy, highGz;

float prevLowAx, prevLowAy, prevLowAz;
float prevHighAx, prevHighAy, prevHighAz;

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

  // Dados Brutos Acelerômetro ---------------------

  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  rawAx = (Wire.read() << 8 | Wire.read());
  rawAy = (Wire.read() << 8 | Wire.read());
  rawAz = (Wire.read() << 8 | Wire.read());

  // -----------------------------------------------

  // Dados Brutos Giroscópio -----------------------

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  rawGx = (Wire.read() << 8 | Wire.read());
  rawGy = (Wire.read() << 8 | Wire.read());
  rawGz = (Wire.read() << 8 | Wire.read());

  // -----------------------------------------------
}

ISR(TIMER2_OVF_vect) {
  read = true;

  TCNT2 = 99;
}

void accelerometerData() {
  readSensor();

  // Converter dados brutos para g
  ax = rawAx / 16384;
  ay = rawAy / 16384;
  az = rawAz / 16384;

  highAx = alpha * ax - (1 - alpha) * prevHighAx;
  highAy = alpha * ay - (1 - alpha) * prevHighAy;
  highAz = alpha * az - (1 - alpha) * prevHighAz;

  prevHighAx = highAx;
  prevHighAy = highAy;
  prevHighAz = highAz;

  lowAx = alpha * highAx + (1 - alpha) * prevLowAx;
  lowAy = alpha * highAy + (1 - alpha) * prevLowAy;
  lowAz = alpha * highAz + (1 - alpha) * prevLowAz;

  prevLowAx = lowAx;
  prevLowAy = lowAy;
  prevLowAz = lowAz;
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
  if(read) {
    read = false;

    accelerometerData();

    Serial.print(ax, 6);
    Serial.print(",");
    Serial.print(lowAx, 6);
    Serial.print(",");
    Serial.print(highAx, 6);
    Serial.println();
  }
}
