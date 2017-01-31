

#include <Wire.h>
#include <DS1307RTC.h>
/*
about set time:
format: year,month,day,week,hour,min,sec
example: 14,03,25,02,13,55,10   2014.03.25 tuesday 13:55:10
*/
String comdata = "";
int mark=0;
//store the current time data
int rtc[7];
//store the set time data
byte rr[7];

//initial light
void setup()
{
  DDRC |= _BV(2) | _BV(3); // POWER:Vcc Gnd
  PORTC |= _BV(3); // VCC PINC3

  //initial baudrate
  Serial.begin(9600);
  //get current time
  RTC.get(rtc, true);
  //if time is wrong reset to default time
  if (rtc[6] < 12) {
    //stop rtc time
    RTC.stop();
    RTC.set(DS1307_SEC, 23);
    RTC.set(DS1307_MIN, 27);
    RTC.set(DS1307_HR, 07);
    RTC.set(DS1307_DOW, 7);
    RTC.set(DS1307_DATE, 11);
    RTC.set(DS1307_MTH, 10);
    RTC.set(DS1307_YR, 14);
    //start rtc time
    RTC.start();
  }
  //RTC.SetOutput(LOW);
  //RTC.SetOutput(HIGH);
  //RTC.SetOutput(DS1307_SQW1HZ);
  //RTC.SetOutput(DS1307_SQW4KHZ);
  //RTC.SetOutput(DS1307_SQW8KHZ);
  RTC.SetOutput(DS1307_SQW32KHZ);
}
 
void loop()
{
  int i;
  //get current time 
  RTC.get(rtc, true);
  //print current time format : year month day week hour min sec
  for (i = 0; i < 7; i++)
  {
    Serial.print(rtc[i]);
    Serial.print(" ");
  }
  //blink the light
  Serial.println();

  delay(500);

  delay(500);
  //
  int j = 0;
  //read all the data
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
  }
  //if data is all collected,then parse it
  if (mark == 1)
  {
    Serial.println(comdata);
    Serial.println(comdata.length());
    //parse data
    for (int i = 0; i < comdata.length() ; i++)
    {
      //if the byte is ',' jump it,and parse next value
      if (comdata[i] == ',')
      {
        j++;
      }
      else
      {
        rr[j] = rr[j] * 10 + (comdata[i] - '0');
      }
    }
    comdata = String("");
    RTC.stop();
    RTC.set(DS1307_SEC, rr[6]);
    RTC.set(DS1307_MIN, rr[5]);
    RTC.set(DS1307_HR, rr[4]);
    RTC.set(DS1307_DOW, rr[3]);
    RTC.set(DS1307_DATE, rr[2]);
    RTC.set(DS1307_MTH, rr[1]);
    RTC.set(DS1307_YR, rr[0]);
    RTC.start();
    mark = 0;
  }
 
}
