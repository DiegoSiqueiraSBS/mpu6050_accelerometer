#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>
#include <MadgwickAHRS.h>

#define MPU 0x68

Madgwick filter;

int i = 0;

float t = 0.00;

float deltat = 0.01;

float alpha = 0.09;
float alphaGyro = 0.09;

float linXCalibration = 0.004305;
float linYCalibration = -0.002111;
float linZCalibration = 0.104113;

float rawAx, rawAy, rawAz;
float rawGx, rawGy, rawGz;

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

  filter.begin(250);
}

void accelerometerCallibration() {

  float lin_ax, lin_ay, lin_az;
  float lin_ax_temp, lin_ay_temp, lin_az_temp;

  readSensor();

  // Converter dados brutos para g
  ax = rawAx / 16384;
  ay = rawAy / 16384;
  az = rawAz / 16384;

  // Converter dados brutos para graus p/ segundo
  gx = rawGx / 131;
  gy = rawGy / 131;
  gz = rawGz / 131;

  // Filtro passa-baixa aceleração
  ax = alpha * ax + (1 - alpha) * prev_ax;
  ay = alpha * ay + (1 - alpha) * prev_ay;
  az = alpha * az + (1 - alpha) * prev_az;

  prev_ax = ax;
  prev_ay = ay;
  prev_az = az;

  // Filtro passa-baixa giroscópio
  gx = alphaGyro * gx + (1 - alphaGyro) * prev_gx;
  gy = alphaGyro * gy + (1 - alphaGyro) * prev_gy;
  gz = alphaGyro * gz + (1 - alphaGyro) * prev_gz;

  prev_gx = gx;
  prev_gy = gy;
  prev_gz = gz;

  // Confirmar valores usando filtro
  filter.updateIMU(gx, gy, gz, ax, ay, az);
  float q0 = filter.q0;
  float q1 = filter.q1;
  float q2 = filter.q2;
  float q3 = filter.q3;

  // Rotacionar vetor gravidade
  float gX = 2 * (q1*q3 - q0*q2);
  float gY = 2 * (q0*q1 + q2*q3);
  float gZ = q0*q0 - q1*q1 - q2*q2 + q3*q3;

  // Remover gravidade
  lin_ax = ax - gX;
  lin_ay = ay - gY;
  lin_az = az - gZ;

  // Filtro passa-baixa para aceleração linear
  lin_ax = alpha * lin_ax + (1 - alpha) * prev_lin_ax;
  lin_ay = alpha * lin_ay + (1 - alpha) * prev_lin_ay;
  lin_az = alpha * lin_az + (1 - alpha) * prev_lin_az;
  prev_lin_ax = lin_ax;
  prev_lin_ay = lin_ay;
  prev_lin_az = lin_az;

  lin_ax_temp += lin_ax;
  lin_ay_temp += lin_ay;
  lin_az_temp += lin_az;

  Serial.print(lin_ax_temp, 6);
  Serial.print(",");
  Serial.print(lin_ay_temp, 6);
  Serial.print(",");
  Serial.print(lin_az_temp, 6);
  Serial.println();

}

void accelerometerData() {
  readSensor();

  // Converter dados brutos para g
  ax = rawAx / 16384;
  ay = rawAy / 16384;
  az = rawAz / 16384;

  // Converter dados brutos para graus p/ segundo
  gx = rawGx / 131;
  gy = rawGy / 131;
  gz = rawGz / 131;

  // Filtro passa-baixa aceleração
  ax = alpha * ax + (1 - alpha) * prev_ax;
  ay = alpha * ay + (1 - alpha) * prev_ay;
  az = alpha * az + (1 - alpha) * prev_az;

  prev_ax = ax;
  prev_ay = ay;
  prev_az = az;

  // Filtro passa-baixa giroscópio
  gx = alphaGyro * gx + (1 - alphaGyro) * prev_gx;
  gy = alphaGyro * gy + (1 - alphaGyro) * prev_gy;
  gz = alphaGyro * gz + (1 - alphaGyro) * prev_gz;

  prev_gx = gx;
  prev_gy = gy;
  prev_gz = gz;

  // Confirmar valores usando filtro
  filter.updateIMU(gx, gy, gz, ax, ay, az);
  float q0 = filter.q0;
  float q1 = filter.q1;
  float q2 = filter.q2;
  float q3 = filter.q3;

  // Rotacionar vetor gravidade
  float gX = 2 * (q1*q3 - q0*q2);
  float gY = 2 * (q0*q1 + q2*q3);
  float gZ = q0*q0 - q1*q1 - q2*q2 + q3*q3;

  // Remover gravidade
  lin_ax = ax - gX;
  lin_ay = ay - gY;
  lin_az = az - gZ;

  // Filtro passa-baixa para aceleração linear
  lin_ax = alpha * lin_ax + (1 - alpha) * prev_lin_ax;
  lin_ay = alpha * lin_ay + (1 - alpha) * prev_lin_ay;
  lin_az = alpha * lin_az + (1 - alpha) * prev_lin_az;

  prev_lin_ax = lin_ax;
  prev_lin_ay = lin_ay;
  prev_lin_az = lin_az;

  lin_ax -= linXCalibration;
  lin_ay -= linYCalibration;
  lin_az -= linZCalibration;

  // X calculus
  if (lin_ax > 0.000000 && lin_ax < 0.001684) {
    lin_ax = 0.00;
    velocityX = 0.00;
  }
  velocityX += ((lin_ax + prev_lin_ax) * 0.5 * deltat) * 9.81;

  // Y calculus
  if(lin_ay > -0.001073	&& lin_ay < -0.000442){
    lin_ay = 0.00;
    velocityY = 0.00;
  }
  velocityY += ((lin_ay + prev_lin_ay) * 0.5 * deltat) * 9.81;
  
  // Z calculus
  if(lin_az > 0.003117 && lin_az < 0.008144){
    lin_az = 0.00;
    velocityZ = 0.00;
  }
  velocityZ += ((lin_az + prev_lin_az) * 0.5 * deltat) * 9.81;

  if(velocityX > 0.000000 && velocityX < 0.000289) {
    velocityX = 0.00;
  }
  if(velocityY > -0.000577 && velocityY < -0.000134) {
    velocityY = 0.00;
  }
  if(velocityZ > -0.753871 && velocityZ < 0.005477) {
    velocityZ = 0.00;
  }

  distanceX += (velocityX + prev_velocityX) * 0.5 * deltat;
  distanceY += (velocityY + prev_velocityY) * 0.5 * deltat;
  distanceZ += (velocityZ + prev_velocityZ) * 0.5 * deltat;

  prev_velocityX = velocityX;
  prev_velocityY = velocityY;
  prev_velocityZ = velocityZ;

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

      Serial.print(lin_ax, 4);+
      Serial.print(",");
      Serial.print(lin_ay, 4);
      Serial.print(",");
      Serial.print(lin_az, 4);
      Serial.println();

      // Calibração Remoção Ruído
      // if (i < 5000) {
      //   accelerometerData();
      //   i ++;
      // }

      // Calibração
      // if (i < 5000) {
      //   accelerometerCallibration();
      //   i ++;
      // }
      
    }
}
