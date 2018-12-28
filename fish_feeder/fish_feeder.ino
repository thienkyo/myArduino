#include <Servo.h>
#include "Sweeper.h"

int closePos = 84 ; //90 go down this num for less food.
int openPos = 20; //50
long cnt = 0L;
byte times = 1;

Sweeper sw(25,openPos,closePos);

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  sw.Attach(9);
  sw.sweepTo(openPos);
  cnt = 86400L;
  Serial.println("****************This was made by kyo****************");
  delay(1000);
}

void feed(){
  sw.sweepTo(closePos);
  sw.sweepTo(openPos);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.print("cnt");
  Serial.println(cnt);
  
  if(cnt == 86400L){
    Serial.println("if");
    cnt = 86396L;
    for(byte i = 1; i <= times ; i++){
      feed();
    }
  }
  cnt++;
  //feed();
}
