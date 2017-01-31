#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif


#define _cs 6
#define _dc 9
#define _rst 8
//#define _miso 74
//#define _mosi 75
//#define _sclk 76
// Using software SPI is really not suggested, its incredibly slow
//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);
// Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
int led = 24;
//
// Hardware configuration
//


RF24 radio(53,52);



// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };



void setup(void)
{
 
    
    
  tft.begin();
  tft.fillScreen(ILI9340_BLACK);
  pinMode(led, OUTPUT); 
  digitalWrite(led, LOW); 


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

  
  radio.printDetails();
  role = role_pong_back;
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
}

void loop(void)
{
 
  
char got_it[31];
 
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      
      bool done = false;
      int len;
      while (!done)
      {
       
        // Fetch the payload, and see if this was the last one.
        len = radio.getDynamicPayloadSize();
        done = radio.read( &got_it, len );
        
        // Spew it
        int lenArray;
        lenArray = strlen(got_it);
        //get volatge
        int voltageStart = 0;
        int voltageEnd = 0;
        int LEDStart = 0;
        int LEDEnd = 0;
        for(int i=0; i<lenArray+1; i++){
          if(got_it[i] == 'S'){
            voltageStart = i + 1;
          }
          
          if(got_it[i] == 'L'){
            LEDStart = i + 1;
            voltageEnd = i - 1;
          }
          
           if(got_it[i] == '-'){
            LEDEnd = i - 1;
          }
            
        }
        String voltage;
        for(int i=voltageStart; i<voltageEnd+1; i++){
          voltage += got_it[i];
        }
        String ledon;
        for(int i=LEDStart; i<LEDEnd+1; i++){
          ledon += got_it[i];
        }
        
      //  Serial.println("Got payload ");
       // Serial.println("Voltage is: ");
       // Serial.print(voltage);
       // Serial.print("LED should be: ");
       // Serial.print(ledon);
       // Serial.println("");
        
        if(ledon == "1"){
          digitalWrite(led, HIGH); 
        }
        else{
         digitalWrite(led, LOW);  
        }
	
        screenText(voltage, ledon);
      }

      // First, stop listening so we can talk
      radio.stopListening();
      
  
      // Send the final one back.
      String message = "Recieved";
      message += got_it;
     char SendPayload[31] = "";
       int messageSize = message.length();
       for (int i = 0; i < messageSize; i++) {
       SendPayload[i] = message.charAt(i);
       }
      
    //Serial.print(strlen(SendPayload));
    //Serial.println("");
      radio.write( &SendPayload, strlen(SendPayload) );
     // p("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

 

 


unsigned long screenText(String voltage, String ledon) {
  //tft.fillScreen(ILI9340_BLACK);
  //tft.tosetTextColor(ILI9340_BLACK)
  //tft.drawRect(0, 0, 240, 20, ILI9340_BLACK);
  //tft.fillRect(0, 0, 240, 50, ILI9340_BLACK);
  //delay(10);
  tft.setTextColor(ILI9340_GREEN, ILI9340_BLACK);  tft.setTextSize(3);
  long randNumber;
   randNumber = random(10, 20);
   tft.setCursor(0, 0);
  tft.println("Got payload ");
  tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK); 
tft.println("Voltage is: ");
tft.println(voltage);
tft.println("");
tft.println("LED is: ");
if(ledon.equals("1")){
tft.println("On ");
}
else{
tft.println("Off");
}
tft.println("");
  //delay(22);
  

}

