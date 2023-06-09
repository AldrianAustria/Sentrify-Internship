//Libraries
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_EC.h"

//Pin Definitions Sensors
#define temp_pin 33
#define sensor_saln 35
#define sensor_ph 34
#define sensor_do 39

//Pin Definitions Button
#define lcd_btn 17
#define mtr_btn 16

//Pin Dinifitions Motor Control
#define mtr_a1 18
#define mtr_a2 19

//Pin Definitions LED
#define mtr_led 27
#define do_led 4
#define sal_led 12
#define ph_led 32
#define tmp_led 25

//Pin Definition buzzer
#define bzr 13

//Pin Definition Relay Switch
#define swtch_saln 15
#define swtch_do 26

#define VREF 3300     //VREF (mv)
#define ADC_RES 4096  //ADC Resolution
#define TWO_POINT_CALIBRATION 0

//Web Server Initalization
WebServer server(80);

//Relay Boolean
int swtch_val = 1;

//LCD Size
int lcdColumns = 16;
int lcdRows = 2;

//Sensors Variables
float dslv_oxgn = 0;
float saln = 0;
float ph_lvl = 0;
float wtr_temp = 0;

float dslv_oxgn_cal = 0;
float saln_cal = 0;
float ph_lvl_cal = 0;
float wtr_temp_cal = 0;

//Temporary Sensors Variables
float temp_saln = 0;
float temp_ph_lvl = 0;

//
uint32_t CAL1_V = 528;
uint32_t CAL1_T = 30.81;

const uint16_t DO_Table[41] = {
  14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
  11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
  9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
  7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Raw_map;
uint16_t ADC_Voltage;
uint16_t DO;

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c) {
#if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = CAL1_V + (uint32_t)35 * temperature_c - CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

//Sensor States
String state_do;
String state_sl;
String state_ph;
String state_tp;
String state_mt;
String state_sys;

//Sensor Calibrate Values
String ph_off;
String volt_off;
String tmp_off;
String rktStatus;

char val[7];
String input;

bool rktState = false;
bool alrmstat = false;

//pH Level Variables
float calibration = 0;
int sensorValue = 0;
float avgValue;
float b;
int buf[10], temp;
double ph_measure = 0;

//Salinity Level Variables
float salnvol = 0;
float ecValue = 0;

//Toggle Button Logic Variable for Motor Control
int mtr_toggle = 1;
int mtr_event = 0;
int mtr_Old_event = 1;

//Toggle Button Logic Variable for LCD Control
int lcd_event = 0;
int lcd_Old_event = 1;
int page = 0;

//Tone Variables
int tone_val = 750;
int tone_low = 750;
int tone_high = 1000;
bool bzr_state = true;

//Millis function delays
unsigned long current_time = 0;
unsigned long prev_time = 0;
unsigned long delay_time = 1000;
unsigned long bzr_prev_time = 0;
unsigned long swtch_delay_time = 20000;
unsigned long prev_time_ph = 0;
unsigned long ph_delay_time = 4000;
unsigned long prev_time_lcd = 0;
unsigned long lcd_delay_time = 1000;

//Object Declaration
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
OneWire oneWire_in_temperature(temp_pin);
DallasTemperature sensor_temperature(&oneWire_in_temperature);
DFRobot_EC ec;

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (file) {
    server.send(200, "text/html", file.readString());
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void handleDisplay() {
  File file = SPIFFS.open("/calibrate.html", "r");
  if (file) {
    server.send(200, "text/html", file.readString());
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void setup() {
  for (int i = 750; i < 1000; i++) {
    bzr_prev_time = current_time;
    tone(bzr, i);
  }

  //Initialize Serial
  Serial.begin(115200);

  //Pinmode
  pinMode(mtr_a1, OUTPUT);
  pinMode(mtr_a2, OUTPUT);
  pinMode(swtch_saln, OUTPUT);
  pinMode(swtch_do, OUTPUT);
  pinMode(lcd_btn, INPUT);
  pinMode(mtr_btn, INPUT);
  pinMode(mtr_led, OUTPUT);
  pinMode(do_led, OUTPUT);
  pinMode(sal_led, OUTPUT);
  pinMode(ph_led, OUTPUT);
  pinMode(tmp_led, OUTPUT);

  //Initialize Dedault Values
  digitalWrite(swtch_saln, HIGH);
  digitalWrite(swtch_do, HIGH);

  //LCD Configuration
  lcd.init();
  lcd.backlight();

  //WiFi Initialization
  WiFi.mode(WIFI_AP);
  WiFi.softAP("REKTfier", "password");

  //WebServer Configuration
  server.on("/", HTTP_GET, handleRoot);
  server.on("/calibrate", handleDisplay);

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

  server.on("/do_cal", []() {
    server.send(200, "text/plain", String(dslv_oxgn_cal));
  });

  server.on("/saln_cal", []() {
    server.send(200, "text/plain", String(saln_cal));
  });

  server.on("/ph_cal", []() {
    server.send(200, "text/plain", String(ph_lvl_cal));
  });

  server.on("/temp_cal", []() {
    server.send(200, "text/plain", String(wtr_temp_cal));
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

  server.on("/sys", []() {
    server.send(200, "text/plain", String(state_sys));
  });

  server.on("/rkt", HTTP_POST, []() {
    rktStatus = server.arg("rktStatus");
    if (rktStatus == "on") {
      rktState = true;
    } else if (rktStatus == "off") {
      rktState = false;
    }
    String stringRkt = rktState ? "ON" : "OFF";
    String json = "{\"rktState\":" + String(rktState) + ",\"stringRkt\":\"" + stringRkt + "\"}";
    server.send(200, "application/json", json);
  });

  server.on("/phoff", HTTP_POST, []() {
    ph_off = server.arg("phnum");
    ph_off.trim();
    calibration = ph_off.toFloat();
    server.send(200, "text/plain");
  });

  server.on("/volt", HTTP_POST, []() {
    volt_off = server.arg("voltnum");
    volt_off.trim();
    CAL1_V = volt_off.toFloat();
    server.send(200, "text/plain");
  });

  server.on("/do_temp", HTTP_POST, []() {
    tmp_off = server.arg("tmpnum");
    tmp_off.trim();
    CAL1_T = tmp_off.toFloat();
    server.send(200, "text/plain");
  });

  server.on("/button1", []() {
    input = "ENTEREC";
    server.send(200, "text/plain", "Button 1 pressed");
  });

  server.on("/button2", []() {
    input = "CALEC";
    server.send(200, "text/plain", "Button 2 pressed");
  });

  server.on("/button3", []() {
    input = "EXITEC";
    server.send(200, "text/plain", "Button 3 pressed");
  });

  //SPIFFS Initalization
  SPIFFS.begin();

  //Server Initialization
  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.softAPIP());

  //Set Temperature Sensor Wait Configuration
  sensor_temperature.setWaitForConversion(false);

  //Initialize Temperature Sensor
  sensor_temperature.begin();
}

//Sensor Functions
float dissolve_o2_read();
float salinity_read();
float ph_level_read();
float temperature_read();

//Alarm Function
void alarm();

//Display Function
void display_value();

float dissolve_o2_cal();
void ec_cal();
float salinity_calread();

void loop() {
  //Millis Function
  current_time = millis();

  //Web Server
  server.handleClient();

  if (rktState == true) {
    state_sys = "ON";
    wtr_temp_cal = 0;
    ph_lvl_cal = 0;
    saln_cal = 0;
    dslv_oxgn_cal = 0;

    //Toggle Button Logic for Motor Control
    if (mtr_event == HIGH && mtr_Old_event == 0) {
      if (mtr_toggle == 0) {
        digitalWrite(mtr_led, LOW);
        digitalWrite(mtr_a1, HIGH);
        digitalWrite(mtr_a2, LOW);
        state_mt = "ON";
        mtr_toggle = 1;
      } else {
        digitalWrite(mtr_led, HIGH);
        digitalWrite(mtr_a1, LOW);
        digitalWrite(mtr_a2, LOW);
        state_mt = "OFF";
        mtr_toggle = 0;
      }
    }
    mtr_Old_event = mtr_event;
    mtr_event = digitalRead(mtr_btn);

    //Toggle Button Logic for Motor Control
    if (lcd_event == HIGH && lcd_Old_event == 0) {
      if (page == 3) {
        page = 0;
      } else {
        page++;
      }
    }
    lcd_Old_event = lcd_event;
    lcd_event = digitalRead(lcd_btn);

    //Relay Switching
    if (current_time - prev_time >= swtch_delay_time) {
      prev_time = current_time;
      if (swtch_val == 1) {
        digitalWrite(swtch_saln, LOW);
        digitalWrite(swtch_do, HIGH);
        swtch_val = 2;
      } else if (swtch_val == 2) {
        digitalWrite(swtch_saln, HIGH);
        digitalWrite(swtch_do, LOW);
        swtch_val = 3;
      } else if (swtch_val == 3) {
        digitalWrite(swtch_saln, HIGH);
        digitalWrite(swtch_do, HIGH);
        swtch_val = 1;
      }
    }

    //Sensor Readings
    wtr_temp = temperature_read();
    temp_ph_lvl = ph_level_read();
    if (swtch_val == 2) {
      saln = salinity_calread();
    } else if (swtch_val == 3) {
      dslv_oxgn = dissolve_o2_read();
    } else if (swtch_val == 1) {
      if (current_time - prev_time_ph >= ph_delay_time) {
        prev_time_ph = current_time;
        ph_lvl = temp_ph_lvl;
      }
    }

    if (current_time - prev_time_lcd >= lcd_delay_time) {
      prev_time_lcd = current_time;

      //Remarks Condition
      if (dslv_oxgn < 4) {
        digitalWrite(do_led, HIGH);
        state_do = "Bad";
      }

      else {
        digitalWrite(do_led, LOW);
        state_do = "Good";
      }

      if (saln < 15 || saln > 25) {
        digitalWrite(sal_led, HIGH);
        state_sl = "Bad";
      }

      else {
        digitalWrite(sal_led, LOW);
        state_sl = "Good";
      }

      if (ph_lvl < 6.5 || ph_lvl > 9) {
        digitalWrite(ph_led, HIGH);
        state_ph = "Bad";
      }

      else {
        digitalWrite(ph_led, LOW);
        state_ph = "Good";
      }

      if (wtr_temp < 25 || wtr_temp > 31) {
        digitalWrite(tmp_led, HIGH);
        state_tp = "Bad";
      }

      else {
        digitalWrite(tmp_led, LOW);
        state_tp = "Good";
      }
      display_value();
    }
    //Alarm
    if (dslv_oxgn < 4 || saln < 15 || saln > 25 || ph_lvl < 6.5 || ph_lvl > 9.0 || wtr_temp < 25 || wtr_temp > 31) {
      alarm();
    }

    else {
      noTone(bzr);
    }
  }

  else {
    state_sys = "OFF";

    //Disable all connected components
    noTone(bzr);
    digitalWrite(mtr_a1, LOW);
    digitalWrite(mtr_a2, LOW);
    digitalWrite(swtch_saln, LOW);
    digitalWrite(swtch_do, LOW);
    digitalWrite(mtr_led, HIGH);

    digitalWrite(do_led, HIGH);
    digitalWrite(sal_led, HIGH);
    digitalWrite(ph_led, HIGH);
    digitalWrite(tmp_led, HIGH);

    state_do = "OFF";
    state_mt = "OFF";
    state_ph = "OFF";
    state_sl = "OFF";
    state_tp = "OFF";

    wtr_temp = 0;
    ph_lvl = 0;
    saln = 0;
    dslv_oxgn = 0;

    mtr_toggle = 0;
    lcd_event = 0;
    page = 0;
    swtch_val = 1;

    lcd.setCursor(0, 0);
    lcd.print("Calibration Mode");
    lcd.setCursor(0, 1);
    lcd.print(" Please Wait!!!");

    if (current_time - prev_time_lcd >= lcd_delay_time) {
      prev_time_lcd = current_time;

      //Read Sensor Data RAW
      wtr_temp_cal = temperature_read();
      ph_lvl_cal = ph_level_read();
      saln_cal = salinity_calread();
      dslv_oxgn_cal = dissolve_o2_cal();

      Serial.print(wtr_temp_cal);
      Serial.print(", ");
      Serial.print(ph_lvl_cal);
      Serial.print(", ");
      Serial.print(saln_cal);
      Serial.print(", ");
      Serial.println(dslv_oxgn_cal);
    }

    //Salinity Sensor Commands
    if (input == "ENTEREC") {
      input.toCharArray(val, 8);
      ec_cal();
      memset(val, '\0', sizeof(val));
      input = "";
    }

    if (input == "CALEC") {
      input.toCharArray(val, 8);
      ec_cal();
      memset(val, '\0', sizeof(val));
      input = "";
    }

    if (input == "EXITEC") {
      input.toCharArray(val, 8);
      ec_cal();
      memset(val, '\0', sizeof(val));
      input = "";
    }
  }
}

void ec_cal() {
  //Salinity Sensor Calibration
  ec.calibration(salnvol, wtr_temp, val);
}

float dissolve_o2_cal() {
  uint32_t raw = analogRead(sensor_do);
  return raw * VREF / ADC_RES;
}

/*float salinity_read() {
  //Get PPT Level
  //ec.calibration(salnvol, wtr_temp);
  int val = analogRead(sensor_saln);
  //int newval = map(val, 0, 4095, 0, 1023) * 2;
  salnvol = val / 4095.0 * 3300;
  ecValue = ec.readEC(salnvol, wtr_temp);
  float ppt = 0.00151342 * pow(ecValue, 2) + 0.578939 * ecValue - 0.223059;
  if (ppt < 0) {
    ppt = 0;
  }
  return ppt;
}*/

float dissolve_o2_read() {
  ADC_Raw = analogRead(sensor_do);
  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;
  return readDO(ADC_Voltage, (uint8_t)wtr_temp) / 1000.0;
}

float salinity_calread() {
  //Read Salinity Values in mS/cm
  ec.calibration(salnvol, wtr_temp);
  /*int val = analogRead(sensor_saln);
  int newval = map(val, 0, 4095, 0, 1023);
  salnvol = newval / 1023.0 * 5000;
  ecValue = ec.readEC(salnvol, wtr_temp);*/
  int val = analogRead(sensor_saln);
  salnvol = val / 4095.0 * 3300;
  ecValue = ec.readEC(salnvol, wtr_temp);
  if(rktState == true){
    float ppt = 0.00151342 * pow((ecValue * 2.22), 2) + 0.578939 * ecValue * 2.22 - 0.223059;
    ppt = ppt + 10;
    if (ppt < 10) {
      ppt = 0;
    }
    return ppt;
  } else {
    return ecValue;
  }
}

float ph_level_read() {
  //Get pH Level
  avgValue = analogRead(sensor_ph);
  float pHVol = avgValue * (3.3 / 4095.0);
  ph_measure = (3.3 * pHVol) + calibration;
  return ph_measure;
}

float temperature_read() {
  //Get Temperature
  sensor_temperature.requestTemperatures();
  return sensor_temperature.getTempCByIndex(0);
}

void display_value() {
  lcd.clear();

  //Debug Screen
  Serial.print(dslv_oxgn);
  Serial.print(", ");
  Serial.print(saln);
  Serial.print(", ");
  Serial.print(ph_lvl);
  Serial.print(", ");
  Serial.println(wtr_temp);

  if (page == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Dissolved O2%: ");
    lcd.setCursor(0, 1);
    lcd.print(dslv_oxgn);
    lcd.print("mg/L");
  }

  if (page == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Salinity Level: ");
    lcd.setCursor(0, 1);
    lcd.print(saln);
    lcd.print("PPT");
  }

  if (page == 2) {
    lcd.setCursor(0, 0);
    lcd.print("pH Level: ");
    lcd.setCursor(0, 1);
    lcd.print(ph_lvl);
    lcd.print("pH");
  }

  if (page == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.setCursor(0, 1);
    lcd.print(wtr_temp);
    lcd.print((char)223);
    lcd.print("C  ");
  }
}

void alarm() {
  //Alarm Logic
  tone(bzr, 500);
}