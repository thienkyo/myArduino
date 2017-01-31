// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void setup () {
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

 // if (! rtc.isrunning()) {
 //   Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
 //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
   //  rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//  }
 // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(13,OUTPUT);
}

void loop () {
    DateTime now = rtc.now();
    
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
    
    //turn on and off led
    if(now.hour() == 6 && now.minute() == 45 && now.second() == 0){
      digitalWrite(13,HIGH);
    }else if(now.hour() == 6 && now.minute() == 55 && now.second() == 0){
      digitalWrite(13,LOW);
    }
    
    if(now.hour() == 16 && now.minute() == 45 && now.second() == 0){
      digitalWrite(13,HIGH);
    }else if(now.hour() == 16 && now.minute() == 55 && now.second() == 0){
      digitalWrite(13,LOW);
    }
    
    delay(500);
    
}
