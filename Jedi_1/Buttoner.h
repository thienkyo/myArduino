#ifndef Buttoner_h
#define Buttoner_h

class Buttoner
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

#endif
