#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include "pitches.h"
#include <Wire.h>
   
// definition of the RFID
RFID rfid(10,5);

byte rfidmipo[5] = {0x26,0x4C,0x53,0x8F,0xB6}; 
byte rfidnum2[5] = {0x44,0xCB,0x94,0x5B,0x40}; 
// put your other cards allowed here

byte serNum[5];
byte data[5];

// definition of melodies access / welcome and rejection / error
int access_melody[] = {NOTE_A4,0,NOTE_A4,0};
int access_noteDurations[] = {8,8,8,8,8,4,8,8,8,8,8,4};
int fail_melody[] = {NOTE_G2,0,NOTE_D2,0};
int fail_noteDurations[] = {8,8,8,8,8,4};

// define pins of the LED, Buzzer and Servo-motor
int LED_access = 2;
int LED_denied = 3;
int speaker_pin = 8;
int servoPin = 9;

// define servo-motor
Servo doorLock;

void setup(){
  doorLock.attach(servoPin); // link servo motor to pin
  Serial.begin(9600); // initialize serial communication
  SPI.begin(); // initialize SPI communication for RFID
  rfid.init(); // initialize the RFID
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  Serial.println("+                    RFID Doorlock (v1)                     +");
  Serial.println("+           Please choose from the following:               +");
  Serial.println("+           - Scan your card                                +");
  Serial.println("+           - Press the button to add new RFID tag#         +");
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  delay(1000);
  pinMode(LED_access,OUTPUT);
  pinMode(LED_denied,OUTPUT);
  pinMode(speaker_pin,OUTPUT);
  pinMode(servoPin,OUTPUT);
}
 
void loop(){

  // Create a variable for each user
  boolean rfidmipo_card = true; // my card
  
  if (rfid.isCard()){ // found valid card
    if (rfid.readCardSerial()){ // reads the card
      delay(1000);
      data[0] = rfid.serNum[0]; // stores the serial number
      data[1] = rfid.serNum[1];
      data[2] = rfid.serNum[2];
      data[3] = rfid.serNum[3];
      data[4] = rfid.serNum[4];
    }
  
  //rfid.halt(); // RFID to standby
 if(data[0] < 16){
    Serial.print("0");
  }
  Serial.print(data[0],HEX);
  
  if(data[1] < 16){
    Serial.print("0");
  }
  Serial.print(data[1],HEX);
  
  if(data[2] < 16){
    Serial.print("0");
  }
  Serial.print(data[2],HEX);
  
  if(data[3] < 16){
    Serial.print("0");
  }
  Serial.print(data[3],HEX);
  
  if(data[4] < 16){
    Serial.print("0");
  }
  Serial.print(data[4],HEX);
  for(int i=0; i<5; i++){
    if(data[i] != rfidmipo[i]) rfidmipo_card = false; // if not my card, poses as fake. Here you can check the other cards allowed
  }
  Serial.println();
  if (rfidmipo_card){ // A card found
    Serial.print("Hello Michael."); //Serial.println("Hello Michael!"); // print message
  }

  else{ // if an unknown card is scanned
    Serial.print("Card not recognized. Access denied."); // Prints message
    digitalWrite(LED_denied, HIGH); //Red LED lights up
    delay(1000); // waiting
    digitalWrite(LED_denied, LOW); // red LED goes out
  }
  if (rfidmipo_card){// add other users with a logical or Welcome message and access permission
    Serial.print(" Access Granted.");
    digitalWrite(LED_access,HIGH); // green LED lights up
    doorLock.write(180); // door releases
    delay(5000); // waiting
    doorLock.write(0); // gate Latch
    digitalWrite(LED_access,LOW); // green LED goes out
  }
  Serial.println();
  delay(500);
  rfid.halt();
  }
}
  

