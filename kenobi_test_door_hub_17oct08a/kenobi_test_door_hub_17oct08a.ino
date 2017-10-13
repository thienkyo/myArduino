#include <Servo.h>
class Sweeper
{
    Servo servo;              // the servo
    int pos;              // current servo position
    int increment;        // increment to move for each interval
    int  updateInterval;      // interval between updates
    unsigned long lastUpdate; // last update of position

  public: Sweeper(int interval)
    {
      updateInterval = interval;
      increment = 2;
    }
  public: Sweeper() {}

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
      if ((millis() - lastUpdate) > updateInterval) // time to update
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

    void toZero()//open
    {
      byte posRead = servo.read();
      if (posRead > 0)
      {
        //Serial.print("in if ");
        for (int i = posRead; i >= 2 ; i -= increment)
        {
          //Serial.print("in if 2");
          lastUpdate = millis();
        //  servo.write(i);
        //  delay(updateInterval);
        }
      //  servo.write(0);
      //  servo.write(11);
        delay(updateInterval);
      }
      posRead = servo.read();
      Serial.print("posRead toZero: ");
      Serial.println(posRead);
    }

    void to180()//close
    {
      byte posRead = servo.read();
      if (posRead > 0)
      {
        for (int i = posRead; i <= 180 ; i += increment)
        {
          lastUpdate = millis();
          servo.write(i);
          delay(updateInterval);
        }
      //  servo.write(180);
        //servo.write(167);
        delay(updateInterval);
      }
      posRead = servo.read();
      Serial.print("posRead to180: ");
      Serial.println(posRead);
    }

    int readPos() {
      return servo.read();
    }

    void writePos(int pos) {
      servo.write(pos);
    }
};

byte servoPin = 9; //lock door servo D9
byte servoInterval = 15;
Sweeper sweeper1(servoInterval); // servo pull the latch
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200); // initialize serial communication
  sweeper1.Attach(servoPin);
  sweeper1.toZero();
}

void loop() {
  // put your main code here, to run repeatedly:

  sweeper1.Update();

  Serial.println("+++++++++++++++++++++++++End loop+++++++++++++++++++++++++++");
}


