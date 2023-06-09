#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define VREF 3300     //VREF(mv)
#define ADC_RES 4096  //ADC Resolution
#define tempsense 33
#define doslv 39

float temp;

uint32_t raw;

OneWire oneWire_in_temperature(tempsense);
DallasTemperature sensor_temperature(&oneWire_in_temperature);

void setup() {
  Serial.begin(115200);
  sensor_temperature.setWaitForConversion(false);

  //Initialize Temperature Sensor
  sensor_temperature.begin();
}

float temperature_read() {
  //Get Temperature
  sensor_temperature.requestTemperatures();
  return sensor_temperature.getTempCByIndex(0);
}

void loop() {
  temp = temperature_read();
  raw = analogRead(doslv);
  Serial.println("raw:\t" + String(raw) + "\tVoltage(mv)\t" + String(raw * VREF / ADC_RES) + "\ttemp:" + String(temp));
  delay(1000);
}