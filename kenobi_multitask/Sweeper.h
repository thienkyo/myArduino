#ifndef backstage_h
#define backstage_h

#include "Arduino.h"

class Sweeper
{
  Servo servo;              // the servo
  byte pos;              // current servo position 
  byte increment;        // increment to move for each interval
  byte  in;           //
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
  byte openPos;
  byte endPos;
  
public: 
  Sweeper(int interval, byte openPosition, byte endPosition)
  {
    updateInterval = interval;
    increment = 4;
    openPos  = openPosition;
    endPos  = endPosition;
  }
  
  void Attach(int pin) { servo.attach(pin);}
  
  void Detach() { servo.detach();}
  
  void Update()
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();
      pos += increment;
      servo.write(pos);
      Serial.println(pos);
      if ((pos >= 180) || (pos <= 0)) // end of sweep
      {
        // reverse direction
        increment = -increment;
      }
    }
  }

  void sweepTo(int Npos)//open
  {
    int in = 1;
    pos = servo.read();
    in = Npos > pos ? increment : -increment;
    for (int i = pos; Npos > pos ? i <= Npos : i >= Npos ; i += in)
    {
      servo.write(i);
      delay(updateInterval);
    }
      
   // posRead = servo.read();
    Serial.print(F("end pos: "));
    Serial.println(pos);
  }

  boolean sweepTo(byte Npos)
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();
      pos = servo.read();
      in = Npos > pos ? increment : -increment;
      pos += in;
      servo.write(pos);
      Serial.println(pos);
      if ((pos >= Npos - increment) && (pos <= Npos + increment)) // end of sweep
      {
        return (false);
      }else{
        return (true);  
      }
    }
  }
};

#endif
