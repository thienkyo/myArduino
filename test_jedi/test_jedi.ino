#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 4);
String rx_data ;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mySerial.available()){
    rx_data = mySerial.readStringUntil('\n');
    Serial.print(F("rx_data: ")); 
    Serial.println(rx_data); 
    //Serial.write(mySerial.read());
  }
}

