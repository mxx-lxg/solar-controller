#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

LiquidCrystal_I2C lcd(0x27, 16, 2); 
//Pins
#define RELAY 13
#define ONE_WIRE 2

//Sensor Objekte
OneWire oneWire(ONE_WIRE);
DallasTemperature sensors(&oneWire);


//Zeitangaben in Sekunden
#define FLUSH_INTERVAL 10L //Wie oft solll im Warte-Zustand geprüft werden
#define FLUSH_DURATION 10L //Pumpen-Laufzeit zur Prüfung
#define PUMP_DURATION 1L //Wartezeit zur nächsten Prüfung/Messung

//Temperatur-Toleranz in °C
#define TURNON_TOLERANCE 3
#define TURNOFF_TOLERANCE 2

//Ausgleich Messwerte
#define CALIBRATION_OFFSET -1

//Temperaturen und Status ausgeben
void printTemp(String state, float in, float out){
    //Serial Ausgabe
    Serial.print("Eingang: ");
    Serial.print(in);
    Serial.print("°C | Ausgang: ");
    Serial.print(out);
    Serial.print("°C | Status: ");
    Serial.println(state);

    //LCD Ausgabe
    lcd.setCursor(0, 0);
    lcd.print("I " + String(in, 1) + (char)223 + "C"); 
    lcd.setCursor(10, 0);
    lcd.print(state); 
    lcd.setCursor(0, 1);
    lcd.print("O " + String(out, 1) + (char)223 + "C"); 
}

//Temperatur Eingang
float getIn(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

//Temperatur Ausgang
float getOut(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(1) + CALIBRATION_OFFSET;
}

void setup() {
  //LCD Init
  lcd.init(); 
  lcd.backlight(); 

  lcd.setCursor(0, 0);
  lcd.print("Solar-Controller"); 
  lcd.setCursor(0, 1);
  lcd.print("by mxx-lxg"); 

  //Relais Pin-Belegung
  pinMode(RELAY, OUTPUT);
  
  //Sensor Initialisierung
  sensors.begin();

  //Serieller Kram für Serial Monitor
  Serial.begin(9600);
  Serial.println("Solar-Controller bereit");

  lcd.setCursor(0, 0);
  lcd.print("Solar-Controller bereit"); 
  lcd.clear();
  //entlüften
  /*Serial.println("entlüften");
  digitalWrite(RELAY, HIGH);
  delay(30000);
  digitalWrite(RELAY, LOW);*/
}


void loop() {
  //flush
  printTemp("sp\365len", getIn(), getOut());
  digitalWrite(RELAY, HIGH);
  delay(FLUSH_DURATION * 1000);
  
  //messen
  float tempIn = getIn();
  float tempOut = getOut(); 
  
  printTemp("fertig", tempIn, tempOut);

  if(tempIn < -100 || tempOut < -100){
    //Fehler Augabe bei Sensor Problemen
    printTemp("Fehler", tempIn, tempOut);
    delay(5000);
    return;
  } else {
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
      
      printTemp("fertig", checkIn, checkOut);
    }
  }

  
  //Warten biszum nächsten Spülen
  digitalWrite(RELAY, LOW);
  delay(FLUSH_INTERVAL * 1000);
}