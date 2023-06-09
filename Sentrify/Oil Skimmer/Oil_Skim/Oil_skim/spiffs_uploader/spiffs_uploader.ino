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
  file.println("text-align: justify;");
  file.println("font-size: 35px;");
  file.println("}");
  file.println("</style>");

  file.println("</head>");

  file.println("<body>");
  file.println("<h1 style=\"font-size:65px; text-align:center\">Water Monitoring System</h1>");

  file.println("<br><br>");

  file.println("<h2>These are the parameters that must be maintain to achieve a good quality water:</h2>");
  file.println("<ul>");
  file.println("<li><h2>Ideal Dissolve Oxygen: Above 4.00mg/L</h2></li>");
  file.println("<li><h2>Ideal Salinity Level: Between 15.00PPT and 25.00PPT</h2></li>");
  file.println("<li><h2>Ideal pH Level: Between 6.50pH and 9.00pH</h2></li>");
  file.println("<li><h2>Ideal Water Temperature: Between 25</span><span>&#176;</span>C and 31</span><span>&#176;</span>C</h2></li>");
  file.println("</ul>");

  file.println("<br><br>");

  file.println("<table style=\"width:100%\"");
  file.println("<tr>");
  file.println("<th>Parameters</th>");
  file.println("<th>Readings</th>");
  file.println("<th>Findings</th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Dissolved Oxygen: </th>");
  file.println("<th><span id=\"dslv_oxgn\"></span>mg/L</th>");
  file.println("<th><span id=\"do_findings\"></span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Salinity Level: </th>");
  file.println("<th><span id=\"saln\"></span>PPT</th>");
  file.println("<th><span id=\"saln_findings\"></span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>pH Level: </th>");
  file.println("<th><span id=\"ph_lvl\"></span>pH</th>");
  file.println("<th><span id=\"ph_findings\"></span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Temperature: </th>");
  file.println("<th><span id=\"wtr_temp\"></span><span>&#176;</span>C</th>");
  file.println("<th><span id=\"temp_findings\"></span></th>");
  file.println("</tr>");
  file.println("</table>");

  file.println("<br>");

  file.println("<h3>Motor Status: <span id=\"mtr_status\"></h3>");

  file.println("<script>");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"dslv_oxgn\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/do_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"saln\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/saln_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"ph_lvl\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/ph_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"wtr_temp\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/temp_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("const doElement = document.getElementById(\"do_findings\");");
  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
//  file.println("document.getElementById(\"do_findings\").innerHTML = this.responseText;");
  file.println("const do_val = this.responseText;");
  file.println("doElement.innerHTML = do_val;");
  file.println("doElement.style.color = do_val === \"Good\" ? \"green\" : \"red\";");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/do_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("const salnElement = document.getElementById(\"saln_findings\");");
  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
//  file.println("document.getElementById(\"saln_findings\").innerHTML = this.responseText;");
  file.println("const saln_val = this.responseText;");
  file.println("salnElement.innerHTML = saln_val;");
  file.println("salnElement.style.color = saln_val === \"Good\" ? \"green\" : \"red\";");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/saln_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("const phElement = document.getElementById(\"ph_findings\");");
  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
//  file.println("document.getElementById(\"ph_findings\").innerHTML = this.responseText;");
  file.println("const ph_val = this.responseText;");
  file.println("phElement.innerHTML = ph_val;");
  file.println("phElement.style.color = ph_val === \"Good\" ? \"green\" : \"red\";");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/ph_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("const tempElement = document.getElementById(\"temp_findings\");");
  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("const tmp_val = this.responseText;");
  file.println("tempElement.innerHTML = tmp_val;");
  file.println("tempElement.style.color = tmp_val === \"Good\" ? \"green\" : \"red\";");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/temp_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"mtr_status\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/mtr_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 100);");

  file.println("</script>");

  file.println("</body>");
  file.println("</html>");

  file.close();

  Serial.println("File created");
}

void loop() {
  // Empty loop
}
