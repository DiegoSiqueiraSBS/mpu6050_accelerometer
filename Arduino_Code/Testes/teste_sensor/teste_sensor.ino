#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>

#define MPU 0x68

int i = 0;

float t = 0.00;

float deltat = 0.01;

float alpha = 0.09;
float alphaGyro = 0.09;

float linXCalibration = 0.004305;
float linYCalibration = -0.002111;
float linZCalibration = 0.104113;

int16_t rawAx, rawAy, rawAz;
int16_t rawGx, rawGy, rawGz;

float ax, ay, az;
float gx, gy, gz;

float lin_ax, lin_ay, lin_az;

float prev_ax = 0.0, prev_ay = 0.0, prev_az = 0.0;
float prev_gx = 0.0, prev_gy = 0.0, prev_gz = 0.0;

float prev_lin_ax = 0.0, prev_lin_ay = 0.0, prev_lin_az = 0.0;

float velocityX, velocityY, velocityZ;
float prev_velocityX = 0.0, prev_velocityY = 0.0, prev_velocityZ = 0.0;
float distanceX, distanceY, distanceZ;

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

void accelerometerData() {
  readSensor();

  // Converter dados brutos para g
  ax = rawAx / 16384.0;
  ay = rawAy / 16384.0;
  az = rawAz / 16384.0;

  // Converter dados brutos para graus p/ segundo
  gx = rawGx / 131.0;
  gy = rawGy / 131.0;
  gz = rawGz / 131.0;

  // filteredValue = rawValue * (1 - alpha) + prevValue * alpha;

  // Filtro passa-baixa

  ax = alpha * ax + (1 - alpha) * prev_ax;
  ay = alpha * ay + (1 - alpha) * prev_ay;
  az = alpha * az + (1 - alpha) * prev_az;

  prev_ax = ax;
  prev_ay = ay;
  prev_az = az;

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

      accelerometerData();

      Serial.print(ax, 6);
      Serial.println();
      
    }
}
