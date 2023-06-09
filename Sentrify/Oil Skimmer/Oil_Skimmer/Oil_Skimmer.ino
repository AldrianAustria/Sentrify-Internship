//Libraries
#include "DFRobot_EC.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

//Pin Definitions
#define mtr_btn 52
#define lcd_btn 53
#define bzr 23
#define ONE_WIRE_BUS_1 22
#define grn_led 50
#define red_led 51
#define RS 24
#define RW 25
#define DB4 26
#define DB5 27
#define DB6 28
#define DB7 29
#define pwr_ph 30
#define pwr_saln 31
#define mtr_a1 7
#define mtr_a2 6
#define sensor_ph A0
#define sensor_saln A1 
#define sensor_do A2

//Object declaration
OneWire oneWire_in_temperature(ONE_WIRE_BUS_1);
DallasTemperature sensor_temperature(&oneWire_in_temperature);
LiquidCrystal display(RS, RW, DB4, DB5, DB6, DB7);
DFRobot_EC ec;

//Sensor Readings
double dslv_oxgn = 0;
double saln = 0;
double ph_lvl = 0;
double wtr_temp = 0;

//PowerSwitch Variables
int pwr_temp = 0;
int pwr_switch = 1;

//pH Level Variables
float calibration = 22.69;
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10], temp;
double ph_measure = 0;

//Salinity Sensor Variables
float salnvol = 0;
float ecValue = 0;

//Buzzer Timer
int buzzer_timer = 75;

//Toggle Button Logic Variable for Motor Control
int mtr_toggle = 1;
int mtr_event = 1;
int mtr_Old_event = 0;

//Toggle Button Logic Variable for LCD Control
int lcd_event = 0;
int lcd_Old_event = 1;
int page = 0;

//millis() based delay
unsigned long current_time = 0;
unsigned long bzr_prev_time = 0;

//Function Declarations
void alarm();
void display_readings(int get_page);
double dissolve_o2_read();
double salinity_read();
double ph_level_read();
double temperature_read();

//Setup Function
void setup(){
  Serial.begin(9600);
  pinMode(grn_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(mtr_btn, INPUT);
  pinMode(lcd_btn, INPUT);
  pinMode(bzr, OUTPUT);
  pinMode(mtr_a1, OUTPUT);
  pinMode(mtr_a2, OUTPUT);
  pinMode(pwr_ph, OUTPUT);
  pinMode(pwr_saln, OUTPUT);
  display.begin(8, 2);
  sensor_temperature.begin();
}

void loop(){
  //millis() start
  current_time = millis();

  //Toggle Button Logic for Motor Control
  if(mtr_event == HIGH && mtr_Old_event == 0){
    if(mtr_toggle == 0){
      Serial.println("ON");
      digitalWrite(grn_led, HIGH);
      digitalWrite(red_led, LOW);
      analogWrite(mtr_a1, 255);
      analogWrite(mtr_a2, 0);
      mtr_toggle = 1;
      //float calibration = 22.41;
      //digitalWrite(saln_en, LOW);
    }
    else{
      Serial.println("OFF");
      digitalWrite(grn_led, LOW);
      digitalWrite(red_led, HIGH);
      analogWrite(mtr_a1, 0);
      analogWrite(mtr_a2, 0);
      mtr_toggle = 0;
      //float calibration = 22.31;
      //digitalWrite(saln_en, HIGH);
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

  //Sensor Power Switching
  /*if(pwr_switch == 1){
    digitalWrite(pwr_ph, HIGH);
    digitalWrite(pwr_saln, LOW);
    saln = saln_tempo;
    display_readings(page);
    delay(1000);
  }
  if(pwr_switch == 0){
    digitalWrite(pwr_ph, LOW);
    digitalWrite(pwr_saln, HIGH);
    ph_lvl = ph_lvl_tempo;
    display_readings(page);
    delay(1000);
  }
  pwr_switch = !(pwr_switch);*/

  //Sensor Reading Functions
  wtr_temp = temperature_read();
  ph_lvl = ph_level_read();
  saln = salinity_read();

  display_readings(page);

  //Condition Block
  if(saln < 15 || saln > 25 || ph_lvl < 6.5 || ph_lvl > 9.0 || wtr_temp < 25 || wtr_temp > 31){
    alarm();

  }
  else{
    noTone(bzr);
  }
}

void alarm(){
  //Alarm Logic
  for(int i = 500; i<1000; i++){
    if(current_time - bzr_prev_time >= buzzer_timer){
        tone(bzr, i);
    }
  }
}

void display_readings(int get_page){

  //Debug Screen
  Serial.print(dslv_oxgn);
  Serial.print(", ");
  Serial.print(saln);
  Serial.print(", ");
  Serial.print(ph_lvl);
  Serial.print(", ");
  Serial.println(wtr_temp);

  //LCD Display Block
  display.clear();
  if(get_page == 0){
    display.setCursor(0, 0);
    display.print("DO%: ");
    display.setCursor(0, 1);
    display.print(dslv_oxgn);
    display.print("mg/L");
  }

  if(get_page == 1){
    display.setCursor(0, 0);
    display.print("Salinity");
    display.setCursor(0, 1);
    display.print(saln);
    display.print("ppt");
  }

  if(get_page == 2){
    display.setCursor(0, 0);
    display.print("pH Level");
    display.setCursor(0, 1);
    display.print(ph_lvl);
    display.print("pH");
  }

  if(get_page == 3){
    display.setCursor(0, 0);
    display.print("H2O Temp");
    display.setCursor(0, 1);
    display.print(wtr_temp);
    display.print((char)223);
    display.print("C  ");
  }
  delay(50);
}

double temperature_read(){
  //Get Temperature
  sensor_temperature.requestTemperatures();
  return sensor_temperature.getTempCByIndex(0);
}

double ph_level_read(){
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
    float pHVol = (float)avgValue * 5.0 / 1023.0 / 6;
    ph_measure = -5.70 * pHVol + calibration;
  }
  return ph_measure;
}

double salinity_read(){
  salnvol = analogRead(sensor_saln) / 1023.0 * 5000;
  ecValue =  ec.readEC(salnvol, wtr_temp) * 0.5;
  ec.calibration(salnvol, wtr_temp);
  return ecValue;
}