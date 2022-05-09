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
#define FLUSH_INTERVAL 10L //Wie oft solll im Warte-Zustand geprüft werden
#define FLUSH_DURATION 5L //Pumpen-Laufzeit zur Prüfung
#define PUMP_DURATION 5L //Wartezeit zur nächsten Prüfung/Messung

//Temperatur-Toleranz in °C
#define TURNON_TOLERANCE 3
#define TURNOFF_TOLERANCE 1

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
  //Relais Pin-Belegung
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

  if(tempOut <= tempIn + TURNON_TOLERANCE){
    //Temperatur niedriger > nichts tun 
    printTemp("warten", tempIn, tempOut);
  } else {
    //Temperatur höher 
    printTemp("heizen", tempIn, tempOut);
    //delay(PUMP_DURATION * 1000); //ohne Zwischenprüfung feste Zeit laufen
    

    //laufen mit Zwischenprüfung (dynamischer)
    float checkIn = getIn();
    float checkOut = getOut(); 

    while(checkOut > checkIn + TURNOFF_TOLERANCE){
      printTemp("heizen", checkIn, checkOut);
      delay(PUMP_DURATION * 1000);
      checkIn = getIn();
      checkOut = getOut(); 
    }
    
    printTemp("heizen fertig", checkIn, checkOut);
  }

  //Warten biszum nächsten Spülen
  digitalWrite(RELAY, LOW);
  delay(FLUSH_INTERVAL * 1000);
}