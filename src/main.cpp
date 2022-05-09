#include <Arduino.h>

//Pins
#define RELAY 13
#define TEMP_IN 2
#define TEMP_OUT 3


//Angaben in Sekunden
#define FLUSH_INTERVAL 10L
#define FLUSH_DURATION 5L
#define PUMP_DURATION 10L

//Temperatur-Toleranz
#define TOLERANCE 1

//Temperaturen und Status ausgeben
void printTemp(char state, float in, float out){
    Serial.print(in);
    Serial.print(" ");
    Serial.print(out);
    Serial.println(state);
}

//Temperatur Eingang
float getIn(){
  return 23;
}

//Temperatur Ausgang
float getOut(){
  return 24;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY, OUTPUT);
  pinMode(TEMP_IN, INPUT);
  pinMode(TEMP_OUT, INPUT);
  Serial.begin(9600);
  Serial.println("Eingang Ausgang Status");
}

void loop() {
  //flush
  printTemp('1', getIn(), getOut());
  digitalWrite(RELAY, HIGH);
  delay(FLUSH_DURATION * 1000);
  
  //messen
  float tempIn = getIn();
  float tempOut = getOut(); 
  printTemp('1', tempIn, tempOut);

  if(tempOut <= tempIn + TOLERANCE){
    //Temperatur niedriger > warten 
    printTemp('0', tempIn, tempOut);
    digitalWrite(RELAY, LOW);
    delay(FLUSH_INTERVAL * 1000);
  } else {
    //Temperatur höher 
    printTemp('2', tempIn, tempOut);
    delay(PUMP_DURATION * 1000);
  }
}