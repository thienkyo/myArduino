// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

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
int fail_safe_time = 180;//second
int reg_temp = 31;  //regular temperate
int reg_hu   = 70;  //regular humidity
bool noon_ind = true;
const int buttonPin = 2;     // the number of the pushbutton pin: D2
const int relayPin =  13; // the number of the LED pin: D13
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int lastButtonState=0;
void setup () {
  Serial.begin(9600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(13,OUTPUT);
  dht.begin();
  digitalWrite(13,LOW);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop () {
    Serial.println("*******************************************************");
    Serial.print("read(13): ");
    Serial.println(digitalRead(relayPin));
    DateTime now = rtc.now();
    unsigned long t1 =  DateTime(now.year(), now.month(), now.day(), 12, 00, 0).unixtime();
    unsigned long t2 =  DateTime(now.year(), now.month(), now.day(), 13, 30, 0).unixtime();
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
    
    // Compute heat index
   // Must send in temp in Fahrenheit!
    float hi = dht.computeHeatIndex(f, h);

    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hi);
    Serial.println(" *F");
    
    Serial.println();
    Serial.print("t1: " );
    Serial.println(t1);
    Serial.print("no: " );
    Serial.println(now.unixtime());
    Serial.print("t2: " );
    Serial.println(t2);
    
    buttonState = digitalRead(buttonPin);
    Serial.println(buttonState);
    // check if the pushbutton is pressed.
    if(lastButtonState != buttonState){
      digitalWrite(relayPin,digitalRead(relayPin) == 0 ? 1 : 0);    
      fail_safe_start_time = now;
    
    }
    lastButtonState = buttonState;
    
    //turn on at 6:30
    if(now.hour() == 6 && now.minute() == 30 && now.second() == 0){
      if(h < reg_hu || t > reg_temp){
        digitalWrite(relayPin,HIGH);
        fail_safe_start_time = now;
      }
    }
    //turn off at 6:47:30
    if(now.hour() == 6 && now.minute() == 33 && now.second() == 0){
      digitalWrite(relayPin,LOW);
    }
    
    //check temp and humidity at noon
    if(now.unixtime() > t1 and now.unixtime() < t2  && noon_ind == true){
      
      if((h < 35 || t > 32) && digitalRead(relayPin) == 0){
        digitalWrite(relayPin,HIGH);
        noon_task_start_time = now;
        fail_safe_start_time = now;
        Serial.println("noon task: on");
        Serial.println(noon_ind);
      }
      

      if(digitalRead(relayPin) == 1  && now.unixtime() == noon_task_start_time.unixtime() + 60){
        digitalWrite(relayPin,LOW);
        noon_ind = false;
        Serial.println("noon task: off");
        Serial.println(noon_ind);
      }
    } 

     //turn on at
    if(now.hour() == 12 && now.minute() == 0 && now.second() == 0 ){
      if(h < reg_hu || t > reg_temp){
        digitalWrite(relayPin,HIGH);
        fail_safe_start_time = now;
      }
    }
    //turn off at 
    if(now.hour() == 12 && now.minute() == 1 && now.second() == 0){
      digitalWrite(relayPin,LOW);
    }
    
    //15:30  
    if(now.hour() == 15 && now.minute() == 30 && now.second() == 0 ){
      if(h < reg_hu || t > reg_temp){
        digitalWrite(relayPin,HIGH);
        fail_safe_start_time = now;
      }
    }
    //turn off at 16:52
    if(now.hour() == 15 && now.minute() == 33 && now.second() == 0){
      digitalWrite(relayPin,LOW);
    }
    
    if(digitalRead(relayPin) == 1 &&  now.unixtime() == fail_safe_start_time.unixtime() + fail_safe_time)
    {
      digitalWrite(relayPin,LOW);
    }
    delay(400);
}
