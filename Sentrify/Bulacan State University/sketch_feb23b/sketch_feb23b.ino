#include <SPIFFS.h>

#include "FS.h"
#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  File file = SPIFFS.open("/index.html", "w");
  if (!file) {
    Serial.println("Failed to create file");
    return;
  }

  file.println("<!DOCTYPE html>");
  file.println("<html>");
  file.println("<head>");

  file.println("<style>");
  file.println("table, th, td{");
  file.println("border: 1px solid black;");
  file.println("text-align: left;");
  file.println("font-size: 35px;");
  file.println("}");
  file.println("h2{");
  file.println("text-align: center;");
  file.println("font-size: 35px;");
  file.println("}");
  file.println("</style>");

  file.println("</head>");

  file.println("<body>");
  file.println("<h1 style=\"font-size: 65px; text-align:center\">Sentrify Internet of Things</h1>");

  file.println("<br>");

  file.println("<h2>DHT11 Temperature and Humidity Sensor Readings</h2>");

  file.println("<table style=\"width: 100%\"");
  file.println("<tr>");
  file.println("<th>Temperature</th>");
  file.println("<th>Humidity</th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th><span id=\"temp\">Loading...</span><span>&#176;</span>C</th>");
  file.println("<th><span id=\"hmdt\">Loading...</span></th>");
  file.println("</tr>");

  file.println("</table>");

  file.println("<br>");

  file.println("<h2>Control Panel</h2>");

  file.println("<h3>Humidifier: <span id=\"hmdtStatus\">OFF</span></h3>");
  file.println("<form>");
  file.println("<button id=\"on-hmd\" type=\"button\" style=\"font-size: 40px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"setHmdState(true)\">On</button>");
  file.println("<button id=\"off-hmd\" type=\"button\" style=\"font-size: 40px; text-align:center; padding: 30px;\" onclick=\"setHmdState(false)\">Off</button>");
  file.println("</form>");

  file.println("<h3>Fan: <span id=\"fanStatus\">OFF</span></h3>");
  file.println("<form>");
  file.println("<button id=\"on-fan\" type=\"button\" style=\"font-size: 40px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"setFanState(true)\">On</button>");
  file.println("<button id=\"off-fan\" type=\"button\" style=\"font-size: 40px; text-align:center; padding: 30px;\" onclick=\"setFanState(false)\">Off</button>");
  file.println("</form>");

  file.println("<h3>Lights: <span id=\"lightStatus\">OFF</span></h3>");
  file.println("<form>");
  file.println("<button id=\"on-light\" type=\"button\" style=\"font-size: 40px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"setLightState(true)\">On</button>");
  file.println("<button id=\"off-light\" type=\"button\" style=\"font-size: 40px; text-align:center; padding: 30px;\" onclick=\"setLightState(false)\">Off</button>");
  file.println("</form>");

  file.println("<script>");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"temp\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/temp\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"hmdt\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/hmdt\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

  file.println("function setHmdState(on) {");
  file.println("var xhr = new XMLHttpRequest();");
  file.println("xhr.open('POST', '/hmd', true);");
  file.println("xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');");
  file.println("xhr.onreadystatechange = function() {");
  file.println("if (xhr.readyState === 4 && xhr.status === 200) {");
  file.println("var response = JSON.parse(xhr.responseText);");
  file.println("document.getElementById('hmdtStatus').innerHTML = response.stringHumid;");
  file.println("document.getElementById('on-button').disabled = response.humidState;");
  file.println("document.getElementById('off-button').disabled = !response.humidState;");
  file.println("}");
  file.println("};");
  file.println("xhr.send('hmdStatus=' + (on ? 'on' : 'off'));");
  file.println("}");

  file.println("function setFanState(on) {");
  file.println("var xhr = new XMLHttpRequest();");
  file.println("xhr.open('POST', '/fan', true);");
  file.println("xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');");
  file.println("xhr.onreadystatechange = function() {");
  file.println("if (xhr.readyState === 4 && xhr.status === 200) {");
  file.println("var response = JSON.parse(xhr.responseText);");
  file.println("document.getElementById('fanStatus').innerHTML = response.stringFan;");
  file.println("document.getElementById('on-').disabled = response.fanState;");
  file.println("document.getElementById('off-').disabled = !response.fanState;");
  file.println("}");
  file.println("};");
  file.println("xhr.send('fanStatus=' + (on ? 'on' : 'off'));");
  file.println("}");

  file.println("function setLightState(on) {");
  file.println("var xhr = new XMLHttpRequest();");
  file.println("xhr.open('POST', '/light', true);");
  file.println("xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');");
  file.println("xhr.onreadystatechange = function() {");
  file.println("if (xhr.readyState === 4 && xhr.status === 200) {");
  file.println("var response = JSON.parse(xhr.responseText);");
  file.println("document.getElementById('lightStatus').innerHTML = response.stringLight;");
  file.println("document.getElementById('on-').disabled = response.lightState;");
  file.println("document.getElementById('off-').disabled = !response.lightState;");
  file.println("}");
  file.println("};");
  file.println("xhr.send('lightStatus=' + (on ? 'on' : 'off'));");
  file.println("}");

  file.println("</script>");

  file.println("</body>");
  file.println("</html>");

  file.close();

  Serial.println("File created");
}

void loop() {
  // Empty loop
}
