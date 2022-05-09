#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Pins
#define RELAY 13
#define TEMP_IN 2
#define TEMP_OUT 3

//Sensor Objekte
OneWire oneWireA(TEMP_IN);
DallasTemperature sensorsA(&oneWireA);

OneWire oneWireB(TEMP_OUT);
DallasTemperature sensorsB(&oneWireB);


//Zeitangaben in Sekunden
#define FLUSH_INTERVAL 10L
#define FLUSH_DURATION 5L
#define PUMP_DURATION 10L

//Temperatur-Toleranz in °C
#define TOLERANCE 3

//Temperaturen und Status ausgeben
void printTemp(String state, float in, float out){
    Serial.print("Eingang: ");
    Serial.print(in);
    Serial.print("°C | Ausgang: ");
    Serial.print(out);
    Serial.print("°C | Status: ");
    Serial.println(state);
}

//Temperatur Eingang
float getIn(){
  sensorsA.requestTemperatures();
  return sensorsA.getTempCByIndex(0);
}

//Temperatur Ausgang
float getOut(){
  sensorsB.requestTemperatures();
  return sensorsB.getTempCByIndex(0);
}

void setup() {
  //Pin-Belegung
  pinMode(RELAY, OUTPUT);
  
  //Sensor Initialisierung
  sensorsA.begin();
  sensorsB.begin();

  //Serieller Kram für Serial Monitor
  Serial.begin(9600);
  Serial.println("Solar-Controller bereit");
}

void loop() {
  //flush
  printTemp("spülen", getIn(), getOut());
  digitalWrite(RELAY, HIGH);
  delay(FLUSH_DURATION * 1000);
  
  //messen
  float tempIn = getIn();
  float tempOut = getOut(); 
  printTemp("spülen fertig", tempIn, tempOut);

  if(tempOut <= tempIn + TOLERANCE){
    //Temperatur niedriger > warten 
    printTemp("warten", tempIn, tempOut);
    digitalWrite(RELAY, LOW);
    delay(FLUSH_INTERVAL * 1000);
  } else {
    //Temperatur höher 
    printTemp("heizen", tempIn, tempOut);
    delay(PUMP_DURATION * 1000);
    
    //TODO Vlt dynamischer?
    
  }
}