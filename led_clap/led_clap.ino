int soundSensor = 2;
int LED = 3;
int LED2 = 4;

void setup() 
{
  Serial.begin(9600);
  pinMode (soundSensor, INPUT);
  pinMode (LED, OUTPUT);
  pinMode (LED2, OUTPUT);
}

void loop()
{
  //delay(20);
  int statusSensor = digitalRead (soundSensor);
  
  Serial.println(statusSensor);
  if (statusSensor == 1)
  {
    digitalWrite(LED, digitalRead(LED)^1);
    digitalWrite(LED2, digitalRead(LED2)^1);
    delay(1000);
 }
}


