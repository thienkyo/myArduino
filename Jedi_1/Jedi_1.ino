#include <Wire.h>
#include <SoftwareSerial.h>
#include "RTClib.h"
#include <EEPROM.h>
/* ============================================
EEPROM memory : 1024 bytes.
  sector1014: the last 10 bytes are reserved ex:
    1023: total count of entries.
    1022: isFirstTime firstTime usage indicator. ??
    1021&1020: switch id 0-255 ex: 1255, 222108, 13 Id of a switch is consist 2 bytes. binding between switch and phone.
    //isDebug.?
    1019&1018: this the code to compare entries between the two.
    1017,1016,1015,1014 : pre-set period of time of active of a particular outlet when using a button. we have 4 outlet(minutes: 255 max)
   
  sector0000-1013: the 1014 bytes are for entry storage.

Communication protocol:
  head = A : sync entries; + check time
  head = B : sync current time;
  head = C : check time;
  head = D : Debug mode: show on the phone data going in and out.
  head = E : compare entries between switch and phone.
  head = F : reset: delete all entries, isFirstTime.
  head = G : check all = C+E
  head = H : check if entries are sycned or not?
        I : regain the entries from switch to phone.
    
  r: response
  Head = rA : response from Sync Entry command. success or fail. if fail, do it again.
  Head = rB : response from Sync Time command. success or fail. if fail, do it again.
  head = rC : response from Check Time command. ok or not
  Head = rD : response from Comparation command. success or fail. if fail, do it again.
  Head = rE : response from check all command.success or fail. if fail, do it again.
  Head = rF : response from Reset command .success or fail. if fail, do it again.

  tail = O : Only/One
  tail = F : First
  tail = M : Middle
  tail = L : Last
    
Entry
  info'no second' = 14x20 = 280(14byte/entry, we have 1014byte storage.)
  info'with second' = 15x20 = 300(15byte/entry, we have 1014byte storage.)
    
  ex: app send 3 entry to switch, it will send 3 times, 1 entry each time with 1500ms in between.
  command Sync.
  rx_Data 1 = "A:1,18,0,0,18,30,0,1,1,1,1,1,1,1,6:F" len = 36
  rx_Data 2 = "A:1,8,5,0,8,8,0,1,1,1,1,1,1,1,6:M" len = 33
  rx_Data 3 = "A:1,20,10,10,20,30,30,1,1,1,1,1,1,1,7:L" len = 39
  Command Sync Real Time.
  rx_Data 4 = "B:2014,10,15,18,30,0:O"
  command Check Time.
  rx_Data 5 = "C:2016,13:O"
  tx_Data 5a= "rC:ok:0" (r: response)
  tx_Data 5b= "rC:1,1,2001,0,0,0:0"

  ":" : delimeter between parts in rawData;
  part 1 : head ex : A, B or C
  part 2 : body ex : 1,8,5,0,8,8,0,1,1,1,1,1,1,1,6(entry) or 2014,10,15,18,30,0(time)
  part 3 : tail ex : F,M,L,O to tell if the entry in rxdata is First, Middle or Last or Only;
===============================================
*/

int addr = 0;
byte cnt = 0;// number of entry

String rx_data ; //incoming data from bT connection. len = 3?
//String entry; //
//String entry_str = "";

byte container[15];

typedef struct{
  byte relayID;
  byte start_hr; byte start_mn; byte start_se;
  byte end_hr; byte end_mn; byte end_se;
  boolean week[7];
  boolean months[12];
  boolean date[31];
 }  EntryObject;

EntryObject enlist[20];//6 entries x 4 switch
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

  //EEPROM.write(1023, 2);
  loadEntry();
}

const int relay1 = 6;
const int relay2 = 7;
const int relay3 = 8;
const int relay4 = 9;
const int button1 = 10; // the number of the pushbutton pin: D10
const int button2 = 11;
const int button3 = 12;
const int button4 = 13;

class ButtonController
{
  int relayPin;
  int buttonPin;
  int buttonState;
  int lastButtonState;
    
  public:
  ButtonController(int bPin, int rPin){
      buttonPin = bPin;
      relayPin = rPin;
      pinMode(buttonPin,INPUT_PULLUP);
      pinMode(relayPin,OUTPUT);
      digitalWrite(relayPin,LOW);
      buttonState = 0;
      lastButtonState = 0;
  }
    
  void doAction(){
      buttonState = digitalRead(buttonPin);
      // check if the pushbutton is pressed.
      if(lastButtonState != buttonState){
        digitalWrite(relayPin,digitalRead(relayPin) ^ 1);
        Serial.print(F("button: "));
        Serial.println(buttonPin);
        Serial.print(F("button state: "));
        Serial.println(digitalRead(buttonPin));
      }
      lastButtonState = buttonState;
  }
};

ButtonController b1(button1,relay1);// the number of relay pin : D6 and pushbutton pin: D10
ButtonController b2(button2,relay2);
//ButtonController b3(button3,relay3);
//ButtonController b4(button4,relay4);

void loop() {
///////button handler
  b1.doAction();
  b2.doAction();
    
//////listen incoming data from a phone.
  if(mySerial.available() > 0) {
      rx_data = mySerial.readStringUntil('\n');  //store data received
      protocolFactory(rx_data);
  }// endif myserial , reading from BT
    
////// where the ON_OFF thing happen/////////
/////////////////////////////////////////////  
  if(cnt > 0){
  now = rtc.now();
    int  weekday  = now.dayOfWeek();
  long now_time = now.hour()*10000L + now.minute()*100 + now.second();
    
//  Serial.print( F("weekday :"));
//  Serial.println( weekday );
    for(int i =0; i < cnt; i++){
         long start_time = enlist[i].start_hr*10000L + enlist[i].start_mn*100 + + enlist[i].start_se;
         long end_time = enlist[i].end_hr*10000L + enlist[i].end_mn*100 + enlist[i].end_se;
        
         for(int y = 0; i < 7; i++){
           if(enlist[i].week[y] == 1 && weekday == i ){
             action(now_time,start_time, 1,enlist[i].relayID);
             action(now_time,end_time, 0,enlist[i].relayID);
           }
         }
    }
  }//end "the ON_OFF thing"
}//end the big loop()

int freeRam(){
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int saveEntry(int init_addr, byte container[14]){
  EntryObject entry = {
    container[0],//relayID
    container[1],container[2],container[3],//start time
    container[4],container[5],container[6],//end time
    {container[7],container[8],container[9],container[10],container[11],container[12],container[13]}//week
  };
 
  EEPROM.put(init_addr, entry);
  init_addr += sizeof(entry);
  return init_addr;
}

void protocolFactory(String rx_data){
  String head = getValue(rx_data, ':', 0);
  String body = getValue(rx_data, ':', 1);
  String tail = getValue(rx_data, ':', 2);
  Serial.print(F("head: "));
  Serial.println(head);
  Serial.print(F("body: "));
  Serial.println(body);
  Serial.print(F("tail: "));
  Serial.println(tail);

  //// check RTC on arduino
  if(head == "C"){
      int yr_client;
      yr_client =getValue(body, ',', 0).toInt();
      now = rtc.now();
      int yr = now.year();
      //    Serial.print(F("yr: "));
      //    Serial.println(yr);
      //    Serial.print(F("yr_client: "));
      //    Serial.println(yr_client;
      if(yr == yr_client){
          mySerial.println(F("rC:ok:O"));
      }else{
          byte mon = now.month();
          byte date= now.day();
          byte hr  = now.hour();
          byte mn  = now.minute();
          byte sec = now.second();
          String time_str = "rC:"+String(date)+','+String(mon)+','+String(yr_client)+','+String(hr)+','+String(mn)+','+String(sec)+":O";
          mySerial.println(time_str);
          //      Serial.print(F("time_str: "));
          //      Serial.println(time_str);
          //      Serial.print(F("mon: "));
          //      Serial.println(mon);
      }
  }// endif  check Time : C

  /////sync time
  if(head == "B"){
      int ti[6];
      for(int i =0; i <=5 ; i++){
          ti[i] =getValue(body, ',', i).toInt();
      }
      rtc.adjust(DateTime(ti[0], ti[1], ti[2], ti[3], ti[4], ti[5]));
  }//endif sync time : B

  ////// sync entries
  if(head == "A"){
      cnt++;
      if(tail == "F" || tail == "O"){
          //eraseEEPROM(); no need to erase
          noInterrupts();
          addr = 0;
          cnt=1;
      }
      for(int y =0; y <= 13; y++){
          container[y] =getValue(body, ',', y).toInt();
      }
      addr = saveEntry(addr,container);
      if(tail == "L" || tail == "O"){
          EEPROM.write(1023, cnt);
          loadEntry();
          interrupts();
      }
  }// endif sync entry : A
}

void action(long now_time, long act_time, byte action, byte relayID){
  //  Serial.println("in if1: ");
  //  Serial.print("now_time: ");
  //  Serial.println(now_time);
  if(now_time == act_time){
      if(digitalRead(relayID) != action){
          digitalWrite(relayID,action);
      }
      //Serial.println(F("in if2"));
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
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loadEntry(){
  Serial.println(F("**********loadEntry*********"));
  cnt = EEPROM.read(1023);
  Serial.print(F("cnt: "));
  Serial.println(cnt);
  Serial.print(F("memory: "));
  Serial.println(freeRam());
  int aa = 0; //EEPROM address to start reading from
  // int tol = 1;
  EntryObject entry;
  for(int i =0; i < cnt; i++){
      EEPROM.get( aa, entry );
      aa += sizeof(entry);
      enlist[i] = entry;
      Serial.print( F("Read custom object from EEPROM: "));
      Serial.println(aa);
      Serial.println(enlist[i].relayID);
      Serial.println(enlist[i].start_hr);
      Serial.println(enlist[i].start_mn);
      Serial.println(enlist[i].start_se);
      Serial.println(enlist[i].end_hr);
      Serial.println(enlist[i].end_mn);
      Serial.println(enlist[i].end_se);
      for(int y = 0; i < 7; i++){
           Serial.println(enlist[i].week[y]);
      }
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
//  Serial.println(now_time);
  for (int i = 0 ; i < 900 ; i++) { // clear eeprom
  EEPROM.write(i, 0);
  }
//  now = rtc.now();
//   now_time = now.hour()*10000L + now.minute()*100 + now.second();
//  Serial.print(F("after erease: "));
//  Serial.println(now_time);
//  Serial.print(F("eraseEEPROM: "));
//  Serial.println(EEPROM.length());
}

