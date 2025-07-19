#include <Wire.h>

const int MPU = 0x68;
float aceX, aceY, aceZ;
float gyroX, gyroY, gyroZ;

float xOffsetCalibration = 155.53;
float yOffsetCalibration = 235.69;
float zOffsetCalibration = 1489.56;

void leitura() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  aceX = (((Wire.read() << 8 | Wire.read()) - xOffsetCalibration) / 16401.43) * 9.8;
  aceY = (((Wire.read() << 8 | Wire.read()) - yOffsetCalibration) / 16455.37) * 9.8;
  aceZ = (((Wire.read() << 8 | Wire.read()) - zOffsetCalibration) / 16744.62) * 9.8;

  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU, 6, true);

  gyroX = ((Wire.read() << 8 | Wire.read()) / 131) + 4;
  gyroY = (Wire.read() << 8 | Wire.read()) / 131;
  gyroZ = (Wire.read() << 8 | Wire.read()) / 131;
}

void setup() {
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
  
  // for (int i = 0; i < 1000; i ++) {
  //   leitura();
  //   Serial.print(gyroX);
  //   Serial.print(",");
  //   delay(1);
  // }
  
}

void loop() {
  
  leitura();
  Serial.print(aceX);
  Serial.print(",");
  Serial.print(aceY);
  Serial.print(",");
  Serial.print(aceZ);
  Serial.println();
  delay(10);

  // leitura();
  // Serial.print(gyroX);
  // Serial.print(",");
  // Serial.print(gyroY);
  // Serial.print(",");
  // Serial.print(gyroZ);
  // Serial.println();
  // delay(10);
  
}
