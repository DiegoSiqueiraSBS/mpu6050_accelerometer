#include <SoftwareSerial.h>
#include "BluetoothSerial.h"

String disp = "Delphos";

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

SoftwareSerial mySerial (13, 15); // Rx Tx
BluetoothSerial SerialBT;

void setup() {

  mySerial.begin(9600);
  SerialBT.begin(disp);

  pinMode(13, INPUT);
  pinMode(15, OUTPUT);
}

void loop() {
  
  if (SerialBT.available() > 0) {
    char c = SerialBT.read();

    switch (c) {
        case '0':
          mySerial.print('0');
          break;
        case '1':
          mySerial.print('1');
          break;
        default:
          break;
      }
  }
}
