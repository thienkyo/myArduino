#include <Servo.h>
class Sweeper
{
  Servo servo;              // the servo
  int pos;              // current servo position 
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
 
public: 
  Sweeper(int interval)
  {
    updateInterval = interval;
    increment = 2;
  }
  
  void Attach(int pin)
  {
    servo.attach(pin);
  }
  
  void Detach()
  {
    servo.detach();
  }
  
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
};

byte servoPin = 9; //lock door servo D9
byte servoInterval = 15;
Sweeper sweeper1(servoInterval); // servo pull the latch
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize serial communication
  sweeper1.Attach(servoPin);
  sweeper1.sweepTo(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  sweeper1.sweepTo(3);
  delay(1000);
  sweeper1.sweepTo(109);
  delay(1000);
  Serial.println("+++++++++++++++++++++++++End loop+++++++++++++++++++++++++++");
}


