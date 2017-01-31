// water system code without println

#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"

#define DHTPIN 7     
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;

DateTime noon_task_start_time;
DateTime fail_safe_start_time; //fail safe
int fail_safe_time = 120;//second
bool noon_ind = true;

void setup () {
  Serial.begin(9600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(13,OUTPUT);
  dht.begin();
  digitalWrite(13,LOW);
}

void loop () {
    DateTime now = rtc.now();
    unsigned long t1 =  DateTime(now.year(), now.month(), now.day(), 12, 30, 0).unixtime();
    unsigned long t2 =  DateTime(now.year(), now.month(), now.day(), 13, 45, 0).unixtime();
    if(now.hour() == 0 && now.minute() == 0 && now.second() == 0){
      noon_ind = true;
    }
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
    
    //turn on at 6:45
    if(now.hour() == 6 && now.minute() == 45 && now.second() == 0){
      if(h < 70 || t > 28){
        digitalWrite(13,HIGH);
        fail_safe_start_time = now;
      }
    }
    //turn off at 6:47:30
    if(now.hour() == 6 && now.minute() == 47 && now.second() == 30){
      digitalWrite(13,LOW);
    }
    
    //check temp and humidity between 11:30 and 13:30
    if(now.unixtime() > t1 and now.unixtime() < t2 && noon_ind == true){
      
      if((h < 35 || t > 36) && digitalRead(13) == 0){
        digitalWrite(13,HIGH);
        noon_task_start_time = now;
        fail_safe_start_time = now;
      }
      
      
      if(digitalRead(13) == 1  && now.unixtime() == noon_task_start_time.unixtime() + 60){
        digitalWrite(13,LOW);
        noon_ind = false;
      }
    } 
    
    //turn on at 16:40
    if(now.hour() == 15 && now.minute() == 55 && now.second() == 0){
      if(h < 70 || t > 28){
        digitalWrite(13,HIGH);
        fail_safe_start_time = now;
      }
    }
    //turn off at 16:52
    if(now.hour() == 15 && now.minute() == 57 && now.second() == 30){
      digitalWrite(13,LOW);
    }
    
    if(digitalRead(13) == 1 &&  now.unixtime() == fail_safe_start_time.unixtime() + fail_safe_time)
    {
      digitalWrite(13,LOW);
    }
    delay(400);
}
