#include <Wire.h>
#include <SoftwareSerial.h>
#include "RTClib.h"
#include <EEPROM.h>

const int relay2 = 6;
const int button2 = 11;
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState2=0;
void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
 // mySerial.begin(9600);

  #ifdef AVR
  Wire.begin();
  #else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
 // rtc.begin();

  pinMode(relay2,OUTPUT);
  digitalWrite(relay2,LOW);
  pinMode(button2, INPUT_PULLUP);
  Serial.println(F("ready: "));
}

void loop() {
  // put your main code here, to run repeatedly:
  //////button 2
buttonState2 = digitalRead(button2);
// check if the pushbutton is pressed.
if(lastButtonState2 != buttonState2){

 // Serial.print(F("digitalRead(relay2): "));
 // Serial.println(digitalRead(relay2));
  digitalWrite(relay2,digitalRead(relay2) == 0 ? 1 : 0); 
  
  Serial.print(F("button2 after: "));
  Serial.println(digitalRead(button2));

  Serial.print(F("digitalRead(relay2): "));
  Serial.println(digitalRead(relay2));
  Serial.println(F("========================="));
  delay(300);
}
lastButtonState2 = buttonState2;
}
