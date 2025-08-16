#include <SoftwareSerial.h>

#define rx 2
#define tx 3
#define saida 4

SoftwareSerial bluetooth(rx, tx); // Rx, Tx

void setup() {
  bluetooth.begin(9600);
  Serial.begin(9600);

  pinMode(rx, INPUT);
  pinMode(tx, OUTPUT);
  pinMode(saida, OUTPUT);

}

void loop() {
  if (bluetooth.available() > 0) {
    String msg = bluetooth.readString();
    Serial.println(msg);
    if (msg == "DelphosStartIgnition") {
      digitalWrite(saida, HIGH);
    } else {
      digitalWrite(saida, LOW);
    }
  }
}
