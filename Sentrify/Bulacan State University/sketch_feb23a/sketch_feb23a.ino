#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include "DHT.h"

#define DHT11PIN 21
#define humid 17
#define fan 16
#define light 32

float hmdt = 0;
float temp = 0;

WebServer server(80);

String hmd_status;
String fan_status;
String light_status;

DHT dht(DHT11PIN, DHT11);
void setup() {
  pinMode(humid, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(light, OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP-32", "");

  server.on("/", HTTP_GET, []() {
    File file = SPIFFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  });

  server.on("/temp", []() {
    server.send(200, "text/plain", String(temp));
  });

  server.on("/hmdt", []() {
    server.send(200, "text/plain", String(hmdt));
  });

  server.on("/hmd", HTTP_POST, []() {
    hmd_status = server.arg("hmdStatus");
    if (hmd_status == "on") {
      digitalWrite(humid, LOW);
    } else if (hmd_status == "off") {
      digitalWrite(humid, HIGH);
    } else {
      server.send(400, "text/plain", "Invalid status");
      return;
    }
    bool humidState = digitalRead(humid);
    String stringHumid = humidState ? "OFF" : "ON";
    String json = "{\"humidState\":" + String(humidState) + ",\"stringHumid\":\"" + stringHumid + "\"}";
    server.send(200, "application/json", json);
  });

  server.on("/fan", HTTP_POST, []() {
    fan_status = server.arg("fanStatus");
    if (fan_status == "on") {
      digitalWrite(fan, LOW);
    } else if (fan_status == "off") {
      digitalWrite(fan, HIGH);
    } else {
      server.send(400, "text/plain", "Invalid status");
      return;
    }
    bool fanState = digitalRead(fan);
    String stringFan = fanState ? "OFF" : "ON";
    String json1 = "{\"fanState\":" + String(fanState) + ",\"stringFan\":\"" + stringFan + "\"}";
    server.send(200, "application/json", json1);
  });

  server.on("/light", HTTP_POST, []() {
    light_status = server.arg("lightStatus");
    if (light_status == "on") {
      digitalWrite(light, LOW);
    } else if (light_status == "off") {
      digitalWrite(light, HIGH);
    } else {
      server.send(400, "text/plain", "Invalid status");
      return;
    }
    bool lightState = digitalRead(light);
    String stringLight = lightState ? "OFF" : "ON";
    String json2 = "{\"lightState\":" + String(lightState) + ",\"stringLight\":\"" + stringLight + "\"}";
    server.send(200, "application/json", json2);
  });
  
  Serial.begin(115200);
  dht.begin();
  SPIFFS.begin();
  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  hmdt = dht.readHumidity();
  temp = dht.readTemperature();
  /*Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("C ");
  Serial.print("Humidity: ");
  Serial.println(hmdt);
  if (status == "on") {
    digitalWrite(ledPin, HIGH);
    //server.send(200, "text/plain", "LED is on");
  } else if (status == "off") {
    digitalWrite(ledPin, LOW);
    //server.send(200, "text/plain", "LED is off");
  } else {
    server.send(400, "text/plain", "Invalid status");
  }*/
}
