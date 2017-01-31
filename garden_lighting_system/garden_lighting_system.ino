// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
//DTR/RST -> GRN
//ATmega328 (3.3V , 8MHz) 5v, 16MHz
const int buttonPin = 12;     // the number of the pushbutton pin: D2
const int relayPin =  2; // the number of the LED pin: D12
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int lastButtonState=0;

//case 1: 16/1 -> 30/4  18:00 - 5:15
    //5:15-> 18:15 offtime == 5*10000+15*100+0 = 51500 -> 18*10000+15*100+00 = 180000 
const int c1_date_start = 116;//16+1*100=116
const int c1_date_end = 430;//30+4*100=430
const long c1_time_start = 51500;
const long c1_time_end = 181500;

    //case 2: 1/5 -> 15/10  18:15 - 5:00
const int c2_date_start = 501;
const int c2_date_end = 1015;
const long c2_time_start = 50000;
const long c2_time_end = 181500;

    //Case 3: 6/10 -> 15/1 17:45 - 5:30  == 16/10 -> 31/12 + 1/1 -> 15/1 (2 parts)
const int c3_date_start_p1 = 1016;
const int c3_date_end_p1 = 1231;
const int c3_date_start_p2 = 101;
const int c3_date_end_p2 = 115;
const long c3_time_start = 53000;
const long c3_time_end = 180000;

void setup () {
  Serial.begin(57600);//115200
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
 // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,LOW);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop () {
    Serial.println("*******************************************************");
    DateTime now = rtc.now();
  
    Serial.println();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.println();
    
    buttonState = digitalRead(buttonPin);
    //Serial.print("read(buttonPin): ");
    //Serial.println(buttonState);
    //Serial.print("read(relayPin): ");
    //Serial.println(digitalRead(relayPin));
    // check if the pushbutton is pressed.
    if(lastButtonState != buttonState){
      digitalWrite(relayPin,digitalRead(relayPin) == 0 ? 1 : 0); 
    }
    lastButtonState = buttonState;
    
    //now
    long now_date = now.month()*100 + now.day();
    long now_time = now.hour()*10000L + now.minute()*100 + now.second();

    Serial.print("now_date: ");
    Serial.print(now_date);
    Serial.println();
    Serial.print("now_time: ");
    Serial.println(now_time);
   
    //case1
    action(c1_date_start, c1_date_end, c1_time_start, c1_time_end, now_date, now_time);
    //case2
    action(c2_date_start, c2_date_end, c2_time_start, c2_time_end, now_date, now_time);
    //case3
    action(c3_date_start_p1, c3_date_end_p1, c3_time_start, c3_time_end, now_date, now_time);
    action(c3_date_start_p2, c3_date_end_p2, c3_time_start, c3_time_end, now_date, now_time);
     
    delay(400);
}

void action(int date_start, int date_end, long time_start, long time_end, int now_date, long now_time){
  if(now_date >= date_start && now_date <= date_end){
      Serial.println("in if1: ");
      Serial.print("now_time: ");
      Serial.println(now_time);
      Serial.print("time_start: ");
      Serial.println(time_start);
      Serial.print("time_end: ");
      Serial.println(time_end);
      if(now_time == time_start){
        digitalWrite(relayPin,LOW);
        Serial.println("in if2");
      }
      if(now_time == time_end){
        digitalWrite(relayPin,HIGH);
      } 
  }
}
