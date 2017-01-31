/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */
#include <SoftwareSerial.h>
int LED = 13;  //LED on Arduino board
byte rx_data = 0; //variable to store receive data
int count = 0;
// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 7;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  //Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  Serial.print(buttonState);
  Serial.print(": ");
  Serial.println(count);
  delay(500);        // delay in between reads for stability
  count = count +1;
  if(Serial.available()) // check if UART receive data
  {
    rx_data = Serial.read();  //store data received
    if(rx_data == 'a')  //if the character received is 'a'
    digitalWrite(LED, HIGH);  
    else if ( rx_data == 'A') //if the character received is 'A'
    digitalWrite(LED, LOW); 
  }
}



