/*******************************
*       Author : thienkyo      *
*                              *
********************************/
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include <Wire.h>
#include <EEPROM.h>

// definition of the RFID
RFID rfid(10, 5);
int addr = 0;
byte cnt = 0;// number of card
byte master_count = 0;
boolean isMaster = false; // current card is master or not.
typedef struct {
  byte data[5];
}  cardData;

cardData cardlist[50];

byte master[5] = {0xE5, 0xCD, 0x09, 0x53, 0x72}; //E5CD095372
byte master2[5] = {0x26, 0x4C, 0x53, 0x8F, 0xB6}; //264C538FB6
// put your other cards allowed
//byte serNum[5];
byte data[5];

// define pins of the LED, Buzzer and Servo-motor
byte LED_access = 4;// green
byte LED_denied = 3;// red
byte LED_master = 2;// blue
byte button = 6;  // button to open the door. D6
byte doorPin = 7; // door sensor D7
byte twistLockPin = 8; // door servo D8
byte servoPin = 9; //lock door servo D9
byte servoInterval = 15;

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
        for (int i = posRead; i >= 9 ; i -= increment)
        {
          //Serial.print("in if 2");
          lastUpdate = millis();
          servo.write(i);
          delay(updateInterval);
        }
        servo.write(14);
        servo.write(11);
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
        for (int i = posRead; i <= 172 ; i += increment)
        {
          lastUpdate = millis();
          servo.write(i);
          delay(updateInterval);
        }
        servo.write(159);
        servo.write(167);
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

class Buttoner
{
    Sweeper sw;
    Sweeper swdoor;
    byte buttonPin;
    byte buttonState;
    byte lastButtonState;
    long lastDebounceTime = 0;  // the last time the output pin was toggled
    long debounceDelay = 50;    // the debounce time; increase if the output
    long debounceDelay2 = 3000; // for door.
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

    void doorCheck() {
      //  buttonState = digitalRead(buttonPin);
      byte reading = digitalRead(buttonPin);
      //Serial.print("Read: ");
      //Serial.println(reading);

      if (reading != lastButtonState) {
        Serial.println("=============");
        Serial.println(reading);
        Serial.println(lastButtonState);
        if (reading == LOW) { // when door close
          Serial.println("LOW");
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

      if (reading == LOW) {
        if ((millis() - lastDebounceTime) > debounceDelay2) {
          if (isOneTime2) {
            if (sw.readPos() < 20) {
              sw.to180();
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

    void doAction2() {
      //  buttonState = digitalRead(buttonPin);
      int reading = digitalRead(buttonPin);
      //Serial.print("Read: ");
      //Serial.println(reading);

      if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
          buttonState = reading;

          // only toggle the LED if the new button state is HIGH
          if (buttonState == HIGH) {
            // set the servo:
            if (sw.readPos() < 100) {
              sw.to180();
              Serial.println(F("close the door"));
            } else {
              sw.toZero();
              Serial.println(F("open the door"));
            }
          }
        }
      }

      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState = reading;
    }

};

Sweeper sweeper1(servoInterval); // servo pull the latch
Sweeper sweeper2(servoInterval); // servo twist the lock notch.
Buttoner b1(button, sweeper1, sweeper2); // the number of relay pin : D6 and pushbutton pin: D10
Buttoner door(doorPin, sweeper1, sweeper2);
void setup()
{
  Serial.begin(9600); // initialize serial communication
  SPI.begin(); // initialize SPI communication for RFID
  rfid.init(); // initialize the RFID
  pinMode(LED_access, OUTPUT);
  pinMode(LED_denied, OUTPUT);
  pinMode(LED_master, OUTPUT);
  //pinMode(doorPin,INPUT_PULLUP);

  ledStatus(4);
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  Serial.println("+                    RFID Doorlock (v1)                     +");
  Serial.println("+           Please choose from the following:               +");
  Serial.println("+           - Scan your card                                +");
  Serial.println("+           - Press the button to add new RFID tag#         +");
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  delay(1000);
  ledStatus(0);
  sweeper1.Attach(servoPin);
  sweeper1.toZero();
  loadcard();
  Serial.println("+++++++++++++++++++++++++End SetUp+++++++++++++++++++++++++++");
}

void loop()
{
  b1.doAction2(); //check the button if it is pressed or not
  door.doorCheck(); //check the door if it is closed or not by the door sensor.
  //sweeper1.Update();
  //Serial.print("pos: ");
  //Serial.println(sweeper1.readPos());

  // Create a variable for each user
  //boolean rfidmipo_card = true; // my card
  //boolean isMaster = true; // master is present

  if (rfid.isCard()) { // found valid card
    if (rfid.readCardSerial()) { // reads the card
      delay(500);
      data[0] = rfid.serNum[0]; // stores the serial number
      data[1] = rfid.serNum[1];
      data[2] = rfid.serNum[2];
      data[3] = rfid.serNum[3];
      data[4] = rfid.serNum[4];
    }

    //rfid.halt(); // RFID to standby
    if (data[0] < 16) {
      Serial.print("0");
    }
    Serial.print(data[0], HEX);

    if (data[1] < 16) {
      Serial.print("0");
    }
    Serial.print(data[1], HEX);

    if (data[2] < 16) {
      Serial.print("0");
    }
    Serial.print(data[2], HEX);

    if (data[3] < 16) {
      Serial.print("0");
    }
    Serial.print(data[3], HEX);

    if (data[4] < 16) {
      Serial.print("0");
    }
    Serial.print(data[4], HEX);
    /*
      for(int i=0; i<5; i++){
      if(data[i] != master[i]) {
        isMaster = false;
        break;
      }
      }*/

    if ((data[0] == master[0] && data[1] == master[1] && data[2] == master[2] && data[3] == master[3] && data[4] == master[4]) ||
        (data[0] == master2[0] && data[1] == master2[1] && data[2] == master2[2] && data[3] == master2[3] && data[4] == master2[4])) {
      isMaster = true;
    } else {
      isMaster = false;
    }

    Serial.println();
    if (isMaster) { // master card found
      ledStatus(1);
      master_count++;
      Serial.print(F("master card present. master_count="));
      Serial.println(master_count);
      delay(500);
      digitalWrite(LED_master, LOW);
      rfid.halt();
      delay(500);
      digitalWrite(LED_master, HIGH);
      delay(500);
      digitalWrite(LED_master, LOW);
      delay(500);
      digitalWrite(LED_master, HIGH);
      if (master_count == 2) { // master swiped 2 time in a roll == delete all cards
        Serial.println(F("delete all"));
        eraseEEPROM();
        EEPROM.write(1023, 0);
        loadcard();
        master_count = 0;
        isMaster = false;
      }
    } else if (master_count == 0) { // key card to open door.
      Serial.println(F("Start: authenticate the card"));
      for (int i = 0; i < cnt; i++) { //loop all the cards in memory
        if (ByteArrayCompare(cardlist[i].data, data, 5)) {
          if (sweeper1.readPos() < 100) {
            sweeper1.to180();
            Serial.println(F("close the door"));
          } else {
            sweeper1.toZero();
            Serial.println(F("open the door"));
          }
          break;
        }
      }
      Serial.println(F("End : authenticate the card"));
    } else if (master_count == 1 && !isMaster) { // add new card.
      for (byte y = 0; y < 5; y++) {
        if (data[y] < 16) {
          Serial.print("0");
        }
        Serial.print(data[y], HEX);
      }
      Serial.println();
      //Serial.print("before:");
      //Serial.println(addr);
      addr = saveCard(addr, data, 5);
      //Serial.print("after:");
      //Serial.println(addr);
      Serial.println(F("add new card successfully"));
      master_count = 0;
      cnt++;
      EEPROM.write(1023, cnt);
      loadcard();
    }

    Serial.println();
    // delay(500);
    rfid.halt();
  }// end read rfid

  //
  if (!isMaster) {
    if (sweeper1.readPos() > 100) {
      ledStatus(3);
    } else {
      ledStatus(2);
    }
  }
}

void ledStatus(byte sta) {
  if (sta == 1) { //master : program
    digitalWrite(LED_master, HIGH);
    digitalWrite(LED_access, LOW);
    digitalWrite(LED_denied, LOW);
  } else if (sta == 2) { //access : open the door
    digitalWrite(LED_master, LOW);
    digitalWrite(LED_access, HIGH);
    digitalWrite(LED_denied, LOW);
  } else if (sta == 3) { //denied : close the door
    digitalWrite(LED_master, LOW);
    digitalWrite(LED_access, LOW);
    digitalWrite(LED_denied, HIGH);
  } else if (sta == 4) { //start or restart.
    digitalWrite(LED_master, HIGH);
    digitalWrite(LED_access, HIGH);
    digitalWrite(LED_denied, HIGH);
  } else { // do nothing: turn off all led
    digitalWrite(LED_master, LOW);
    digitalWrite(LED_access, LOW);
    digitalWrite(LED_denied, LOW);
  }
}

int saveCard(int init_addr, byte data[], int n) {
  cardData card = { {data[0], data[1], data[2], data[3], data[4]} };
  for (byte y = 0; y < 5; y++) {
    if (card.data[y] < 16) {
      Serial.print("0");
    }
    Serial.print(card.data[y], HEX);
  }
  Serial.println();

  EEPROM.put(init_addr, card);
  init_addr += sizeof(card);
  return init_addr;
}

void loadcard() {
  Serial.println(F("**********loadCard*********"));
  cnt = EEPROM.read(1023);
  Serial.print(F("cnt: "));
  Serial.println(cnt);
  Serial.print(F("memory: "));
  Serial.println(freeRam());
  int aa = 0; //EEPROM address to start reading from
  // int tol = 1;
  cardData card;
  for (byte i = 0; i < cnt; i++) {
    EEPROM.get( aa, card );
    aa += sizeof(card);
    cardlist[i] = card;
    Serial.print( F("Read custom object from EEPROM: "));
    Serial.println(aa);
    for (byte y = 0; y < 5; y++) {
      if (cardlist[i].data[y] < 16) {
        Serial.print("0");
      }
      Serial.print(cardlist[i].data[y], HEX);
    }
    Serial.println();
  }
  addr = aa;
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void eraseEEPROM() {
  for (int i = 0 ; i < addr + 5 ; i++) { // clear eeprom
    EEPROM.write(i, 0);
  }
  addr = 0;
}

boolean ByteArrayCompare(byte a[], byte b[], int array_size)
{
  for (int i = 0; i < array_size; ++i)
    if (a[i] != b[i])
      return (false);
  return (true);
}

