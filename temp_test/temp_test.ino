#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
   pinMode(7,INPUT_PULLUP);
   noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 34286;            // preload timer 65536-16MHz/256/2Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all
}

void loop() {
  // read the state of the pushbutton value:thien test
  
}
  
ISR(TIMER1_OVF_vect)          // timer compare interrupt service routine
{
  //TCNT1 = 34286; 
  byte reading = digitalRead(7);
  Serial.print("Read door: ");
  Serial.println(reading);
}
 

int freeRam(){
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
