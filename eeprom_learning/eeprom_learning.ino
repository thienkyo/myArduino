/*
 * EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;
byte bb;

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);

  EEPROM.get(address,bb);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  Serial.print("EEPROM.length=");
  Serial.print(EEPROM.length());
  Serial.println();
  Serial.print("EEPROM.get=");
  Serial.print(bb);
  Serial.println();
  
  address = address + 1;
  if (address == EEPROM.length()) {
    address = 0;
  }

  delay(500);
}
