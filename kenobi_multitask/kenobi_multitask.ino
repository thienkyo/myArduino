#include <Servo.h>
#include "Sweeper.h"
#include "Buttoner.h"

boolean isOpenRun = false;
boolean isCloseRun = false;
byte button = 6;  // button to open the door. D6

byte closePos1 = 127; //3
byte openPos1 = 3; //109
byte closePos2 = 127; //3
byte openPos2 = 3; //109

Sweeper sweeper1(15,0,00);
Sweeper sweeper2(25,0,90);
Buttoner b1(button, sweeper1, sweeper2); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sweeper1.Attach(8);
  sweeper2.Attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  //sweeper1.sweepTo(90);
  //sweeper2.sweepTo(170);
  b1.doAction(isOpenRun, isCloseRun); //check the button if it is pressed or not

  if(isOpenRun){
    if(sweeper1.sweepTo(openPos1) && sweeper2.sweepTo(openPos2)){
      isOpenRun = false;
      Serial.println(F("open the door"));
    }
  }

  if(isCloseRun){
    if(sweeper1.sweepTo(closePos1) && sweeper2.sweepTo(closePos2)){
      isCloseRun = false;
      Serial.println(F("open the door"));
    }
  }
  
}
