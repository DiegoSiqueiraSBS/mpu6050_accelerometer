#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define MPU 0x68

#define sinalizacao 8

#define chipSelect 10
#define rx 3
#define tx 2
#define saida 4

#define sdConec 500
#define sdEBlue 300
#define sdEBlueEIgni 100

int stateFlag = 0;

float tempo = 0;
float tempoAnterior = 0;

File myFile;

float rawAx, rawAy, rawAz;
float rawGx, rawGy, rawGz;

float ax, ay, az;
float gx, gy, gz;

bool state = false;

volatile bool read = false;

SoftwareSerial bluetooth(rx, tx); // Rx, Tx

String msg;

void sdCardInit() {
	
  Serial.print("Inicializando cartao SD...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Falha na inicializacao!");
    stateFlag = 0;
    delay(5000);
	return;
  }
  Serial.println("Inicializacao bem-sucedida.");
  stateFlag = 1;
  
  if (myFile) {
	myFile.println("-----------------");
	myFile.close();
  }
  
}

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

  // Converter dados brutos para º/s
  gx = rawGx / 131;
  gy = rawGy / 131;
  gz = rawGz / 131;
}

void setup() {
  Serial.begin(115200);
  bluetooth.begin(9600);

  pinMode(sinalizacao, OUTPUT);
  pinMode(rx, INPUT);
  pinMode(tx, OUTPUT);
  pinMode(saida, OUTPUT);
  
  sdCardInit();

  inicialization();

  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
  TIMSK2 = 1<<TOIE2;
  TCNT2 = 99;

  sei();

}

void loop() {

  tempo = millis();

  if (bluetooth.available() > 0) {
    msg = bluetooth.readString();
  }

  if (msg == "conOn") {
    stateFlag = 2;
  } 
  if (msg == "conOff") {
    stateFlag = 1;
  }

  switch (stateFlag) {
    case 0:
      digitalWrite(sinalizacao, LOW);
      break;

    case 1:
      if (((tempo - tempoAnterior) > sdConec)) {
        if (state == HIGH) {
          digitalWrite(sinalizacao, HIGH);
          digitalWrite(sinalizacao, LOW);
        }
        if (state == LOW) {
          digitalWrite(sinalizacao, LOW);
        }
        state = !state;
        tempoAnterior = tempo;
      }
      
      break;

    case 2:
      if (((tempo - tempoAnterior) > sdEBlue)) {
        if (state == HIGH) {
          digitalWrite(sinalizacao, HIGH);
          digitalWrite(sinalizacao, LOW);
          digitalWrite(sinalizacao, HIGH);
        }
        if (state == LOW) {
          digitalWrite(sinalizacao, LOW);
        }

        if (msg == "DelphosStartIgnition") {
          digitalWrite(saida, HIGH);
          stateFlag = 3;
        }

      state = !state;
      tempoAnterior = tempo;
      }

      break;

    case 3:
      if (((tempo - tempoAnterior) > sdEBlueEIgni)) {
        if (state == HIGH) {
          digitalWrite(sinalizacao, HIGH);
        }
      tempoAnterior = tempo;
      }

      if (msg == "DelphosNoIgnition") {
        digitalWrite(saida, LOW);
        stateFlag = 2;
      } 
      break;

    default:
      break;
    }

  if(read) {
    read = false;
	
	accelerometerData();
	
	myFile = SD.open("Log.txt", FILE_WRITE);
	
	if (myFile) {
		myFile.println(String(ax, 6) + "," + String(ay, 6) + "," + String(az, 6) + "," + String(gx, 6) + "," + String(gy, 6) + "," + String(gz, 6));
		myFile.close();
	}
	
    Serial.println(String(ax, 6) + "," + String(ay, 6) + "," + String(az, 6) + "," + String(gx, 6) + "," + String(gy, 6) + "," + String(gz, 6));
  }
}