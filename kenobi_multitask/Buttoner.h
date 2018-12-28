#ifndef Buttoner_h
#define Buttoner_h

class Buttoner
{
    Sweeper sw;
    Sweeper swdoor;
    byte buttonPin;
    byte buttonState;
    byte lastButtonState;
    long lastDebounceTime = 0;  // the last time the output pin was toggled
    long debounceDelay = 50;    // the debounce time; increase if the output
    long doorSensorCloseTime = 3000; // for door sensor to close the door automatically.
    boolean isOneTime = true;
    boolean isOneTime2 = true;

  public:
    Buttoner(byte bPin, Sweeper sweeper, Sweeper swD) {
      buttonPin = bPin;
      sw = sweeper;
      swdoor = swD;
      pinMode(buttonPin, INPUT_PULLUP);
      buttonState = 0;
      lastButtonState = 0;
    }
    //check the door if it is closed or not by the door sensor.
    //automatically close the door
    void doorCheck() {
      //  buttonState = digitalRead(buttonPin);
      byte reading = digitalRead(buttonPin);

      //reset lastDebounceTime
      if (reading != lastButtonState) {
        Serial.println("=============");
        Serial.println(reading);
        Serial.println(lastButtonState);
        if (reading == LOW) { // when door close
          ///Serial.println("LOW");
          if (isOneTime) {
            Serial.println("onetime");
            // reset the debouncing timer
            lastDebounceTime = millis();
            isOneTime = false;
          }

        } else { // door open.
          isOneTime = true;
          isOneTime2 = true;
        }
      }
      // close the door after doorSensorCloseTime 
      if (reading == LOW) {
        if ((millis() - lastDebounceTime) > doorSensorCloseTime) {
          if (isOneTime2) {
            if (sw.readPos() < 20) {
             // sw.to180();
             // swdoor.sweepTo(doorServoClosePos);
              Serial.println("door state: close");
            }
            isOneTime2 = false;
          }
        }
      }

      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState = reading;
    }

    //
    void doAction(boolean& isOpenRun, boolean& isCloseRun) {
      int reading = digitalRead(buttonPin);

      if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
          buttonState = reading;

          // only toggle the LED if the new button state is HIGH
          if (buttonState == HIGH) {
            isOpenRun = !isOpenRun;
            isCloseRun = !isOpenRun;

       /*     
            if (sw.readPos() < 100) {
              isOpenRun = false;
              isCloseRun = true;
              Serial.println(F("close the door"));
            } else {
              isOpenRun = true;
              isCloseRun = false;
              Serial.println(F("open the door"));
            }
        */    
          }
        }
      }

      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState = reading;
    }

};

#endif
