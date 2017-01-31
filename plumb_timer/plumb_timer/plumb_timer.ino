#include <Wire.h>
#include "RTClib.h"

const int relayPin =  13;
DateTime now;
RTC_DS1307 rtc;

//case 1: 16/1 -> 30/4  18:00 - 5:15
//5:15-> 18:15 offtime == 5*10000+15*100+0 = 51500 -> 18*10000+15*100+00 = 180000 

const long start_time1 = 64500;
const long end_time1   = 65500;

const long start_time2 = 164500;
const long end_time2   = 165500;

//const long start_time3 = 173500;
//const long end_time3 = 173700;

void setup() {
    Serial.begin(9600);
    #ifdef AVR
    Wire.begin();
    #else
    Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
    #endif
    rtc.begin();
    pinMode(relayPin,OUTPUT);
    digitalWrite(relayPin,LOW);
//    delay(2000);
//    digitalWrite(relayPin,LOW);
}

void loop() {
    now = rtc.now();
    long now_time = now.hour()*10000L + now.minute()*100 + now.second();
   // Serial.print(F("now_time: "));
   // Serial.println(now_time);
    action(now_time,start_time1, 1,relayPin);
    action(now_time,end_time1, 0,relayPin);

    action(now_time,start_time2, 1,relayPin);
    action(now_time,end_time2, 0,relayPin);

//    action(now_time,start_time3, 1,relayPin);
//    action(now_time,end_time3, 0,relayPin);

    delay(400);
}


void action(long now_time, long act_time, byte action, byte relayID){
    //  Serial.println("in if1: ");
      Serial.print("now_time: ");
      Serial.println(now_time);
      
      if(now_time == act_time){
        digitalWrite(relayID,action);
        Serial.println(F("in if2"));
      }
}
