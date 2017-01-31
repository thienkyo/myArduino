#include <Wire.h>
#include <SoftwareSerial.h>
#include "RTClib.h"
#include <EEPROM.h>
/* ============================================
Receive
head = A : sync entries;
head = B : sync current time;
head = C : check system;

body = entry : entry;
body = time  :
body = check system;

tail = O : Only
tail = F : First
tail = M : Middle
tail = L : Last

ex: app send 3 entry to switch, it will send 3 times, 1 entry each time with 1500ms in between
info = 14x20 = 280
rx_Data 1 = "A:1,18,0,18,30,1,1,1,1,1,1,1,25,6:F" len = 35
rx_Data 2 = "A:1,8,5,8,8,1,1,1,1,1,1,1,5,6:M" len = 31
rx_Data 3 = "A:1,20,10,20,30,1,1,1,1,1,1,1,25,7:L" len = 36

rx_Data 4 = "B:2014,10,15,18,30,0:O"

rx_Data 5 = "C:2016,13:O"

":" : delimeter between parts in rawData;
part 1 : head ex: A, B or C
part 2 : body ex : 1,8,5,8,8,1,1,1,1,1,1,1,5,6(entry) or 2014,10,15,18,30,0(time)
part 3 : tail  ex : F,M,L,O to tell if the entry in rxdata is First, Middle or Last or Only;

===============================================
*/
const int relay1 = 7;
const int relay2 = 6;
const int relay3 = 8;
const int relay4 = 9;
const int button1 = 12; // the number of the pushbutton pin: D10
const int button2 = 11;
const int button3 = 12;
const int button4 = 13;
// variables will change:
int buttonState1 = 0;         // variable for reading the pushbutton status
int lastButtonState1=0;
int buttonState2 = 0;         // variable for reading the pushbutton status
int lastButtonState2=0;
int buttonState3 = 0;         // variable for reading the pushbutton status
int lastButtonState3=0;
int buttonState4 = 0;         // variable for reading the pushbutton status
int lastButtonState4=0;
int addr = 0;
int cnt = 0;// number of entry

String rx_data ; //incoming data from bT connection. len = 35
//String entry; // 
//String entry_str = "";

byte container[14];

typedef struct{
  byte isActive;
  byte start_hr;
  byte start_mn;
  byte end_hr;
  byte end_mn;
  byte mon;
  byte tue;
  byte wen;
  byte thu;
  byte fri;
  byte sat;
  byte sun;
  byte date;
  byte relayID;
 }  EntryObject;

EntryObject enlist[21];
DateTime now;
SoftwareSerial mySerial(3, 4);// RX = pin D3, TX = pin D4
RTC_DS1307 rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);

  #ifdef AVR
  Wire.begin();
  #else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
  #endif
  rtc.begin();

  pinMode(relay1,OUTPUT);
  digitalWrite(relay1,LOW); 
  pinMode(relay2,OUTPUT);
  digitalWrite(relay2,LOW); 
  pinMode(relay3,OUTPUT);
  digitalWrite(relay3,LOW); 
  pinMode(relay4,OUTPUT);
  digitalWrite(relay4,LOW);  

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  //EEPROM.write(1023, 2);
  loadEntry();
}

void loop() {
//////button 1
buttonState1 = digitalRead(button1);
// check if the pushbutton is pressed.
if(lastButtonState1 != buttonState1){
  digitalWrite(relay1,digitalRead(relay1) == 0 ? 1 : 0); 
  Serial.print(F("button1: "));
  Serial.println(digitalRead(button1));
}
lastButtonState1 = buttonState1;

//////button 2
buttonState2 = digitalRead(button2);
// check if the pushbutton is pressed.
if(lastButtonState2 != buttonState2){

  Serial.print(F("digitalRead(relay2): "));
  Serial.println(digitalRead(relay2));
  digitalWrite(relay2,digitalRead(relay2) == 0 ? 1 : 0); 

  Serial.print(F("digitalRead(relay2) after: "));
  Serial.println(digitalRead(relay2));
  
  Serial.print(F("button2: "));
  Serial.print(button2);
  Serial.print(relay2);
  Serial.println(digitalRead(button2));
}
lastButtonState2 = buttonState2;

//////listen incoming data from a phone.
  if(mySerial.available() > 0) {
    rx_data = mySerial.readStringUntil('\n');  //store data received
   // delay(100);
    Serial.print(F("rx_data: "));
    Serial.println(rx_data);
    if(rx_data == "on")  //if the character received is 'on'
    digitalWrite(relay1, HIGH);
    
    if ( rx_data == "off"){ //if the character received is 'off'
      digitalWrite(relay1, LOW);
      mySerial.println(F("ok just sent off"));
    }

    String head = getValue(rx_data, ':', 0);
    String body = getValue(rx_data, ':', 1);
    String tail= getValue(rx_data, ':', 2);
    Serial.print(F("head: "));
    Serial.println(head);
    Serial.print(F("body: "));
    Serial.println(body);
    Serial.print(F("tail: "));
    Serial.println(tail);

    //// check system
    if(head == "C"){
      int chk[2];
      for(int i =0; i <=1 ; i++){
        chk[i] =getValue(body, ',', i).toInt();
      }
      
      now = rtc.now();
      int yr = now.year();
//      Serial.print(F("yr: "));
//      Serial.println(yr);
//      Serial.print(F("chk[0]: "));
//      Serial.println(chk[0]);
      if(yr == chk[0]){
        mySerial.println(F("rC:ok:0"));
      }else{
        byte mon = now.month();
        byte date= now.day();
        byte hr  = now.hour();
        byte mn  = now.minute();
        byte sec = now.second();
        String time_str = "rC:"+String(date)+','+String(mon)+','+String(yr)+','+String(hr)+','+String(mn)+','+String(sec)+":0";
        String time_str2 = ",";
        mySerial.println(time_str);
       
//        Serial.print(F("time_str: "));
//        Serial.println(time_str);
//        Serial.print(F("time_str2: "));
//        Serial.println(time_str2);
//        Serial.print(F("mon: "));
//        Serial.println(mon);
      }
    }// endif  check system : C

    /////sync time
    if(head == "B"){
      int ti[6];
//    now = rtc.now();
//    Serial.print(now.year(), DEC);
//    Serial.print('/');
//    Serial.print(now.month(), DEC);
//    Serial.print('/');
//    Serial.print(now.day(), DEC);
//    Serial.print(' ');
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.second(), DEC);
//    Serial.println();
//    Serial.println();
      for(int i =0; i <=5 ; i++){
        ti[i] =getValue(body, ',', i).toInt();
      }
      rtc.adjust(DateTime(ti[0], ti[1], ti[2], ti[3], ti[4], ti[5]));
//    now = rtc.now();
//    Serial.print(now.year(), DEC);
//    Serial.print('/');
//    Serial.print(now.month(), DEC);
//    Serial.print('/');
//    Serial.print(now.day(), DEC);
//    Serial.print(' ');
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.second(), DEC);
//    Serial.println();
//    Serial.println();
    }//endif sync time : B

    ////// sync entries
    if(head == "A"){
      cnt++;
      if(tail == "F" || tail == "O"){
        eraseEEPROM();
        addr = 0;
        cnt=1;
      }
      
      for(int y =0; y <= 13; y++){
        container[y] =getValue(body, ',', y).toInt();
        //Serial.println(container[y]);
      }
      addr = saveEntry(addr,container[0],container[1],container[2],container[3],
      container[4],container[5],container[6],container[7],container[8],container[9],container[10]
      ,container[11],container[12],container[13]);

      if(tail == "L" || tail == "O"){
        EEPROM.write(1023, cnt);
        loadEntry();
      }
    }// endif sync entry : A
  }// endif myserial , reading from BT
  
////// where the ON_OFF thing happen/////////
/////////////////////////////////////////////  
  if(cnt >= 0){
    now = rtc.now();
    long now_time = now.hour()*10000L + now.minute()*100 + now.second();
    int  weekday  = now.dayOfWeek();
//    Serial.print( F("weekday :"));
//    Serial.println( weekday );
    for(int i =0; i < cnt; i++){
        long start_time = enlist[i].start_hr*10000L + enlist[i].start_mn*100 ;
        long end_time = enlist[i].end_hr*10000L + enlist[i].end_mn*100 ; 
        if(enlist[i].mon == 1 && weekday ==1){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].tue == 1 && weekday ==2){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].wen == 1 && weekday ==3){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].thu == 1 && weekday ==4){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].fri == 1 && weekday ==5){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].sat == 1 && weekday ==6){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
        if(enlist[i].sun == 1 && weekday ==0){
          action(now_time,start_time, 1,enlist[i].relayID);
          action(now_time,end_time, 0,enlist[i].relayID);
        }
    }
  }
}

int freeRam(){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int saveEntry(int init_addr, byte isActive, byte start_hr, byte start_min, byte end_hr, byte end_min
           , byte mon, byte tue, byte wen, byte thu, byte fri, byte sat, byte sun, byte date, byte relayID){
  //Data to store.
  EntryObject entry = {
    isActive,
    start_hr,
    start_min,
    end_hr,
    end_min,
    mon,
    tue,
    wen,
    thu,
    fri,
    sat,
    sun,
    date,
    relayID
  };
  EEPROM.put(init_addr, entry);
  init_addr += sizeof(entry);
  return init_addr;
}

void action(long now_time, long act_time, byte action, byte relayID){
    //  Serial.println("in if1: ");
    //  Serial.print("now_time: ");
    //  Serial.println(now_time);
      if(now_time == act_time){
        digitalWrite(relayID,action);
        Serial.println(F("in if2"));
      }
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loadEntry(){
  Serial.println(F("**********loadEntry*********"));
  cnt = EEPROM.read(1023);
  Serial.print(F("cnt: "));
  Serial.println(cnt);
  Serial.print(F("memory: "));
  Serial.println(freeRam());
  int aa = 0; //EEPROM address to start reading from
  int tol = 1;
  EntryObject entry;
  for(int i =0; i < cnt; i++){
    EEPROM.get( aa, entry );
    aa += sizeof(entry);
    enlist[i] = entry;
    Serial.print( F("Read custom object from EEPROM: "));
    Serial.println( aa);
    Serial.println( enlist[i].isActive );
    Serial.println( enlist[i].start_hr );
    Serial.println( enlist[i].start_mn );
    Serial.println( enlist[i].end_hr );
    Serial.println( enlist[i].end_mn );
    Serial.println(enlist[i].mon);
    Serial.println(enlist[i].tue);
    Serial.println(enlist[i].wen);
    Serial.println(enlist[i].thu);
    Serial.println(enlist[i].fri);
    Serial.println(enlist[i].sat);
    Serial.println(enlist[i].sun);
    Serial.println(enlist[i].date);
    Serial.println(enlist[i].relayID);
  }  

  int buttonState1 = digitalRead(button1);
  int buttonState2 = digitalRead(button2);
  int buttonState3 = digitalRead(button3);
  int buttonState4 = digitalRead(button4);
  
  Serial.print(F("button state 1 2 3 4: "));
  Serial.println(String(buttonState1) + ' '  +String(buttonState2)+' '+buttonState3 + ' ' +buttonState4);
  
  Serial.print(F("count: "));
  Serial.println(cnt);
  Serial.print(F("memory: "));
  Serial.println(freeRam());
  now = rtc.now();
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
  Serial.println(F("**********End of loadEntry*********"));
}

void eraseEEPROM(){
//  now = rtc.now();
//  long now_time = now.hour()*10000L + now.minute()*100 + now.second();
//  Serial.print(F("before erease: "));
//    Serial.println(now_time);
  for (int i = 0 ; i < 300 ; i++) { // clear eeprom
    EEPROM.write(i, 0);
  }
//  now = rtc.now();
//   now_time = now.hour()*10000L + now.minute()*100 + now.second();
//  Serial.print(F("after erease: "));
//    Serial.println(now_time);
//  Serial.print(F("eraseEEPROM: "));
//    Serial.println(EEPROM.length());
}

