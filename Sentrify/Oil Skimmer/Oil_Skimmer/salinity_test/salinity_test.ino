/*
 * file DFRobot_EC.ino
 * @ https://github.com/DFRobot/DFRobot_EC
 *
 * This is the sample code for Gravity: Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1.0), SKU: DFR0300.
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterec -> enter the calibration mode
 *   calec -> calibrate with the standard buffer solution, two buffer solutions(1413us/cm and 12.88ms/cm) will be automaticlly recognized
 *   exitec -> save the calibrated parameters and exit from calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-03-21
 */
#include "DFRobot_EC.h"
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define swtch_saln 15

#define EC_PIN 35
float voltage = 0;
float ecValue = 0;
float temperature = 0;
DFRobot_EC ec;
float analValue = 0;

const int SENSOR_PIN = 33;  // Arduino pin connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);             // setup a oneWire instance
DallasTemperature tempSensor(&oneWire);  // pass oneWire to DallasTemperature library

void setup() {
  pinMode(swtch_saln, OUTPUT);
  Serial.begin(115200);
  tempSensor.setWaitForConversion(false);
  tempSensor.begin();  // initialize the sensor
  digitalWrite(swtch_saln, LOW);
}

void loop() {
  analValue = analogRead(EC_PIN);
  voltage = map(analValue, 0, 4095, 0, 1023) / 1023 * 2150;  // read the voltage
  Serial.print("Voltage: ");
  Serial.println(voltage);
  temperature = readTemperature();            // read your temperature sensor to execute temperature compensation
  ecValue = ec.readEC(voltage, temperature);  // convert voltage to EC with temperature compensation
  Serial.print("temperature:");
  Serial.print(temperature, 1);
  Serial.print("^C  EC:");
  Serial.print(ecValue);
  Serial.println("ms/cm");
  ec.calibration(voltage, temperature);  // calibration process by Serail CMD
}

float readTemperature() {
  tempSensor.requestTemperatures();  // send the command to get temperatures
  return tempSensor.getTempCByIndex(0);
}
