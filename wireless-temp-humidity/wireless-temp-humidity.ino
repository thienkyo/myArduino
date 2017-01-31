/*
  this code is used to do the test : 
  wireless (RF 433mhz ) serial port from arduino to RPI
 */
#include <SoftwareSerial.h>
#include "DHT.h"
#define LED 13  //LED on Arduino board
String rx_data ; //variable to store receive data
int count = 0;
SoftwareSerial mySerial(2, 3); // RX = pin D2, TX = pin D3
// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 7;
int a = 1;
#define DHTPIN 8     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
  mySerial.begin(9600);
  dht.begin();
  pinMode(LED, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
    // Wait a few seconds between measurements.
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  mySerial.print("H"); 
  mySerial.print(h);
  //mySerial.print(" %\t");
  mySerial.print("T"); 
  mySerial.print(t);
  //mySerial.print(" *C ");
  //mySerial.print(f);
  //mySerial.print(" *F\t");
  //mySerial.print("Heat index: ");
  //mySerial.print(hi);
  //mySerial.print(" *F\t");
 // mySerial.print("a: ");
  mySerial.println(a++);
  /*
  if(mySerial.available()) // check if UART receive data
  {
    rx_data = mySerial.readStringUntil('\n');  //store data received
    Serial.print("rx_data: ");
    Serial.println(rx_data);
    if(rx_data == "on")  //if the character received is 'on'
    digitalWrite(LED, HIGH);
    
    if ( rx_data == "off") //if the character received is 'off'
    digitalWrite(LED, LOW); 
    
    if ( rx_data == "semi") //if the character received is 'off'
    digitalWrite(LED, digitalRead(LED) == 0 ? 1 : 0);

  }
 */
 delay(2000);
}



