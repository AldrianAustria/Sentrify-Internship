#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

/*LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(SENSOR_PIN);             // setup a oneWire instance
DallasTemperature tempSensor(&oneWire);  // pass oneWire to DallasTemperature library*/

float calibration_value = 22.69 -1.57;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
//float temperature = 0;
//const int SENSOR_PIN = 22;               // Arduino pin connected to DS18B20 sensor's DQ pin

void setup() {
  Serial.begin(9600);
  /*lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   Welcome to      ");
  lcd.setCursor(0, 1);
  lcd.print(" Circuit Digest    ");
  delay(2000);
  lcd.clear();
  tempSensor.begin();  // initialize the sensor*/
}
void loop() {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1023.0 / 6;
  float ph_act = -5.70 * volt + calibration_value;
  /*lcd.setCursor(0, 0);
  lcd.print("pH Val:");
  lcd.setCursor(8, 0);
  lcd.print(ph_act);
  Serial.print(ph_act);
  Serial.print(", ");
  tempSensor.requestTemperatures();  // send the command to get temperatures
  Serial.println(tempSensor.getTempCByIndex(0));*/
  Serial.println(ph_act);
}