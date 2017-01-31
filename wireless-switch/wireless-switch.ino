/*
  this code is used to do the test : 
  wireless (RF 433mhz ) serial port from arduino to RPI
 */
#include <SoftwareSerial.h>

#define LED 13  //LED on Arduino board
String rx_data ; //variable to store receive data
int count = 0;
//SoftwareSerial mySerial(2, 3); // RX = pin D2, TX = pin D3


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //pinMode(pushButton, INPUT);
  //mySerial.begin(9600);
  pinMode(LED, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  if(Serial.available()) // check if UART receive data
  {
    rx_data = Serial.readStringUntil('\n');  //store data received
    //Serial.print("rx_data: ");
    //Serial.println(rx_data);
    if(rx_data == "on")  //if the character received is 'on'
    digitalWrite(LED, HIGH);
    
    if ( rx_data == "off") //if the character received is 'off'
    digitalWrite(LED, LOW); 
    
    if ( rx_data == "semi") //if the character received is 'off'
    digitalWrite(LED, digitalRead(LED) == 0 ? 1 : 0);

  }

 //delay(2000);
}



