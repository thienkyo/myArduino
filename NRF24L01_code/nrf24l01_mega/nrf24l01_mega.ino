#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
int led = 6;
String message;
String voltageVal;
const int buttonPin = 30; 
char sentvalues[16];
int ledOn = 0;
int buttonState = 0;  


RF24 radio(49,53);//  ce 49 & csn 53

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


void setup(void)
{
  //
  // Print preamble
  //
  pinMode(buttonPin, INPUT);  
  pinMode(led, OUTPUT); 

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  
  radio.openReadingPipe(1,pipes[1]);

  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.startListening();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.printDetails();
}

void loop(void)
{

  digitalWrite(led, LOW); 
char SendPayload[31] = "";
  
  if (role == role_ping_out)
  {
    voltageVal = "0";
  int sensorValue = analogRead(A0);
   float voltage = sensorValue * (5.0 / 1023.0);
   buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {     
    // turn LED on:    
    ledOn = 1;  
  } 
  else {
    // turn LED off:
    ledOn = 0;
  }
char tmp[4];

  
 
dtostrf(voltage, 4, 2, tmp);
voltageVal += tmp;
message = "S" + voltageVal + "L" + ledOn + "-";   
int charToSend[16];
int messageSize = message.length();
 for (int i = 0; i < messageSize; i++) {
   SendPayload[i] = message.charAt(i);
  }  

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
   
    bool ok = radio.write( &SendPayload, strlen(SendPayload) );
    
 
    radio.startListening();

    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 560 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
     // printf("Failed, response timed out.\n\r");
   //   digitalWrite(led, HIGH);
  //delay(1000);
    }
    else
    {
      
      
      bool done = false;
      int len = 0;
       char got_it[31];
      while (!done)
      {
      // Grab the response, compare, and send to debugging spew
      
       len = radio.getDynamicPayloadSize();
      done = radio.read( &got_it, len );

      // Spew it
   //   Serial.println(got_it);
    }
    //delay(20);
      
    }

    // Try again 1s later
    delay(1000);
  }



}

