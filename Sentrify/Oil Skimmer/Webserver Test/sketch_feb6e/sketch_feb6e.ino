//Libraries
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Pin Definitions
#define temp_pin 26
#define lcd_btn 14
#define mtr_btn 33
#define mtr_grn_led 23
#define mtr_red_led 5
#define mtr_a1 18
#define mtr_a2 19
#define sensor_ph 34
#define do_red_led 17
#define do_grn_led 16
#define sal_red_led 27
#define sal_grn_led 25
#define ph_red_led 32
#define ph_grn_led 12
#define tmp_red_led 4
#define tmp_grn_led 2

//Web Server Initalization
WebServer server(80);

//LCD Size
int lcdColumns = 16;
int lcdRows = 2;

//Sensors Variables
float dslv_oxgn = 0;
float saln = 0;
float ph_lvl = 0;
float wtr_temp = 0;

//Sensor States
String state_do;
String state_sl;
String state_ph;
String state_tp;
String state_mt;

//pH Level Variables
float calibration = 20.71;
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10], temp;
double ph_measure = 0;

//Toggle Button Logic Variable for Motor Control
int mtr_toggle = 1;
int mtr_event = 1;
int mtr_Old_event = 0;

//Toggle Button Logic Variable for LCD Control
int lcd_event = 0;
int lcd_Old_event = 1;
int page = 0;

unsigned long current_time = 0;
unsigned long prev_time = 0;
unsigned long delay_time = 1000;

//Object Declaration
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
OneWire oneWire_in_temperature(temp_pin);
DallasTemperature sensor_temperature(&oneWire_in_temperature);

void setup() {
  Serial.begin(115200);

  pinMode(lcd_btn, INPUT);
  pinMode(mtr_btn, INPUT);
  pinMode(mtr_grn_led, OUTPUT);
  pinMode(mtr_red_led, OUTPUT);
  pinMode(do_grn_led, OUTPUT);
  pinMode(do_red_led, OUTPUT);
  pinMode(sal_grn_led, OUTPUT);
  pinMode(sal_red_led, OUTPUT);
  pinMode(ph_grn_led, OUTPUT);
  pinMode(ph_red_led, OUTPUT);
  pinMode(tmp_grn_led, OUTPUT);
  pinMode(tmp_red_led, OUTPUT);

  lcd.init();          
  lcd.backlight();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("REKTfier", "password");

  server.on("/", []() {
    File file = SPIFFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/do_sensor", []() {
    server.send(200, "text/plain", String(dslv_oxgn));
  });

  server.on("/saln_sensor", []() {
    server.send(200, "text/plain", String(saln));
  });

  server.on("/ph_sensor", []() {
    server.send(200, "text/plain", String(ph_lvl));
  });

  server.on("/temp_sensor", []() {
    server.send(200, "text/plain", String(wtr_temp));
  });

  server.on("/do_state", []() {
    server.send(200, "text/plain", String(state_do));
  });

  server.on("/saln_state", []() {
    server.send(200, "text/plain", String(state_sl));
  });

  server.on("/ph_state", []() {
    server.send(200, "text/plain", String(state_ph));
  });

  server.on("/temp_state", []() {
    server.send(200, "text/plain", String(state_tp));
  });

  server.on("/mtr_state", []() {
    server.send(200, "text/plain", String(state_mt));
  });

  SPIFFS.begin();
  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.softAPIP());

  sensor_temperature.begin();
}

float dissolve_o2_read();
float salinity_read();
float ph_level_read();
float temperature_read();

void display_value();

void loop() {
  //Millis Function
  current_time = millis();

  //Web Server
  server.handleClient();

  //Toggle Button Logic for Motor Control
  if(mtr_event == HIGH && mtr_Old_event == 0){
    if(mtr_toggle == 0){
      digitalWrite(mtr_grn_led, HIGH);
      digitalWrite(mtr_red_led, LOW);
      analogWrite(mtr_a1, 255);
      analogWrite(mtr_a2, 0);
      state_mt = "ON";
      mtr_toggle = 1;
      //float calibration = 22.41;
    }
    else{
      digitalWrite(mtr_grn_led, LOW);
      digitalWrite(mtr_red_led, HIGH);
      analogWrite(mtr_a1, 0);
      analogWrite(mtr_a2, 0);
      state_mt = "OFF";
      mtr_toggle = 0;
      //float calibration = 22.31;
    }
  }
  mtr_Old_event = mtr_event;
  mtr_event = digitalRead(mtr_btn);

  //Toggle Button Logic for Motor Control
  if(lcd_event == HIGH && lcd_Old_event == 0){
    if(page == 3){
      page = 0;
    }
    else{
      page++;
    }
  }
  lcd_Old_event = lcd_event;
  lcd_event = digitalRead(lcd_btn);

  //Sensor Readings
  wtr_temp = temperature_read();
  ph_lvl = ph_level_read();

  if(dslv_oxgn < 4){
    digitalWrite(do_red_led, HIGH);
    digitalWrite(do_grn_led, LOW);
    state_do = "Bad";
  }

  else{
    digitalWrite(do_red_led, LOW);
    digitalWrite(do_grn_led, HIGH);
    state_do = "Good";
  }

  if(saln < 15 || saln > 25){
    digitalWrite(sal_red_led, HIGH);
    digitalWrite(sal_grn_led, LOW);
    state_sl = "Bad";
  }

  else{
    digitalWrite(sal_red_led, LOW);
    digitalWrite(sal_grn_led, HIGH);
    state_sl = "Good";
  }

  if(ph_lvl < 6.5 || ph_lvl > 9){
    digitalWrite(ph_red_led, HIGH);
    digitalWrite(ph_grn_led, LOW);
    state_ph = "Bad";
  }

  else{
    digitalWrite(ph_red_led, LOW);
    digitalWrite(ph_grn_led, HIGH);
    state_ph = "Good";
  }

  if(wtr_temp < 25 || wtr_temp > 31){
    digitalWrite(tmp_red_led, HIGH);
    digitalWrite(tmp_grn_led, LOW);
    state_tp = "Bad";
  }

  else{
    digitalWrite(tmp_red_led, LOW);
    digitalWrite(tmp_grn_led, HIGH);
    state_tp = "Good"; 
  }

  //LCD Display
  display_value();
}

float ph_level_read(){
  for(int i = 0; i < 10; i++){ 
    buf[i] = analogRead(sensor_ph);
    delay(30);
  }
  for(int i = 0; i < 9; i++){
    for(int j = i + 1; j < 10; j++){
      if(buf[i] > buf[j]){
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue=0;
  for(int i = 2 ; i < 8; i++){
    avgValue += buf[i];
    float pHVol = (float)avgValue * 3.3 / 4096.0 / 6;
    ph_measure = -5.70 * pHVol + calibration;
  }
  return ph_measure;
}

float temperature_read(){
  //Get Temperature
  sensor_temperature.requestTemperatures();
  Serial.println(wtr_temp);
  return sensor_temperature.getTempCByIndex(0);
}

void display_value(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP: ");
  lcd.print(WiFi.softAPIP());

  if(page == 0){
    lcd.setCursor(0, 1);
    lcd.print("DO%: ");
    lcd.print(dslv_oxgn);
    lcd.print("mg/L");
  }

  if(page == 1){
    lcd.setCursor(0, 1);
    lcd.print("Saline: ");
    lcd.print(saln);
    lcd.print("ppt");
  }

  if(page == 2){
    lcd.setCursor(0, 1);
    lcd.print("pH Lvl: ");
    lcd.print(ph_lvl);
    lcd.print("pH");
  }

  if(page == 3){
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(wtr_temp);
    lcd.print((char)223);
    lcd.print("C  ");
  }
  delay(50);
}
