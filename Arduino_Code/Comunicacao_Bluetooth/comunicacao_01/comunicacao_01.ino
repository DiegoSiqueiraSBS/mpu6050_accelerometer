#include <avr/io.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial (10, 11); // Rx Tx

int main() {
  init();

  Serial.begin(9600);
  mySerial.begin(9600);

  DDRB |= (1 << PB1);  // LED - 9
  DDRB &= ~(1 << PB2); //Rx - 10
  DDRB |= (1 << PB3);  //Tx - 11

  while (1) {
    if (mySerial.available() > 0) {
      char c = mySerial.read();
      Serial.println(c);

      switch (c) {
        case '0':
          PORTB &= ~(1 << PB1);
          break;
        case '1':
          PORTB |= (1 << PB1);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}