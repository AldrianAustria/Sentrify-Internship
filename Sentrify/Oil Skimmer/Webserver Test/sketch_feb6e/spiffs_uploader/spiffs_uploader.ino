#include "FS.h"
#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  File file1 = SPIFFS.open("/calibrate.html", "w");
  if (!file1) {
    Serial.println("Failed to create file");
    return;
  }

  file1.println("<!DOCTYPE html>");
  file1.println("<html>");
  file1.println("<head>");

  file1.println("<title>REKTfier</title>");

  file1.println("<style>");
  file1.println("h2{");
  file1.println("justify-content: center;");
  file1.println("display: flex;");
  file1.println("font-size: 45px;");
  file1.println("}");

  file1.println("table, th, td{");
  file1.println("border: 1px solid black;");
  file1.println("text-align: left;");
  file1.println("font-size: 35px;");
  file1.println("}");

  file1.println("h3{");
  file1.println("text-align: left;");
  file1.println("font-size: 30px;");
  file1.println("}");

  file1.println("</style>");

  file1.println("</head>");

  file1.println("<body>");
  file1.println("<h1 style=\"font-size: 65px; text-align:center\">Water Monitoring System Calibration Page</h1>");

  file1.println("<br>");

  file1.println("<h2>REKTfier System Status: <span id=\"rktStatus\">Loading...</span></h2>");
  file1.println("<form style=\"display:flex; justify-content:center;\">");
  file1.println("<button id=\"on-rkt\" type=\"button\" style=\"font-size: 25px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"setRktState(true)\">On</button>");
  file1.println("<button id=\"off-rkt\" type=\"button\" style=\"font-size: 25px; text-align:center; padding: 30px;\" onclick=\"setRktState(false)\">Off</button>");
  file1.println("</form>");

  file1.println("<br>");

  file1.println("<h2>pH Sensor Calibration:</h2>");
  file1.println("<form id=\"phform\" onsubmit=\"return submitPHOffSet()\" style=\"font-size:35px; display:flex; justify-content:center;\">");
  file1.println("<label for=\"phlabel\">pH Offset Value:</label>");
  file1.println("<input type=\"number\" id=\"phvalue\" name=\"phlabel\" step=\".01\" style=\"font-size:35px; width:250px;\">");
  file1.println("<button type=\"submit\" style=\"font-size: 25px;\">Submit</button>");
  file1.println("</form>");

  file1.println("<br>");

  file1.println("<h2>Salinity Sensor Calibration:</h2>");
  file1.println("<div style=\"display:flex; justify-content:center;\">");
  file1.println("<button id=\"on-\" type=\"button\" style=\"font-size: 25px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"enterec()\">Calibrate ON</button>");
  file1.println("<button id=\"off-\" type=\"button\" style=\"font-size: 25px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"calcec()\">Calibrate SET</button>");
  file1.println("<button id=\"off-\" type=\"button\" style=\"font-size: 25px; text-align:center; padding: 30px;\" onclick=\"exitec()\">Calibrate OFF</button>");
  file1.println("</div>");

  file1.println("<br>");

  file1.println("<h2>Dissolved Oxygen Sensor Calibration:</h2>");
  file1.println("<form id=\"voltform\" onsubmit=\"return submitNewVoltage()\" style=\"font-size:35px; display:flex; justify-content:center;\">");
  file1.println("<label for=\"voltlabel\">Stable Voltage Reading:</label>");
  file1.println("<input type=\"number\" id=\"voltvalue\" name=\"pvoltlabel\" step=\".01\" style=\"font-size:35px; width:250px;\">");
  file1.println("<button type=\"submit\" style=\"font-size: 25px;\">Submit</button>");
  file1.println("</form>");
  file1.println("<br>");
  file1.println("<br>");
  file1.println("<form id=\"tmpform\" onsubmit=\"return submitNewTemp()\" style=\"font-size:35px; display:flex; justify-content:center;\">");
  file1.println("<label for=\"tmplabel\">DO Temperature:</label>");
  file1.println("<input type=\"number\" id=\"tmpvalue\" name=\"tmplabel\" step=\".01\" style=\"font-size:35px; width:250px;\">");
  file1.println("<button type=\"submit\" style=\"font-size: 25px;\">Submit</button>");
  file1.println("</form>");

  file1.println("<br>");
  file1.println("<br>");
  file1.println("<br>");
  file1.println("<br>");

  file1.println("<h2>Parameter Readings for Calibration:</h2>");
  file1.println("<table style=\"width: 100%\"");
  file1.println("<tr>");
  file1.println("<th>Parameters</th>");
  file1.println("<th>Readings</th>");
  file1.println("</tr>");

  file1.println("<tr>");
  file1.println("<th>Dissolved Oxygen: </th>");
  file1.println("<th><span id=\"dslv_oxgn\">Loading...</span>mV</th>");
  file1.println("</tr>");

  file1.println("<tr>");
  file1.println("<th>Salinity Level: </th>");
  file1.println("<th><span id=\"saln\">Loading...</span>mS/cm</th>");
  file1.println("</tr>");

  file1.println("<tr>");
  file1.println("<th>pH Level: </th>");
  file1.println("<th><span id=\"ph_lvl\">Loading...</span>pH</th>");
  file1.println("</tr>");

  file1.println("<tr>");
  file1.println("<th>Temperature: </th>");
  file1.println("<th><span id=\"wtr_temp\">Loading...</span><span>&#176;</span>C</th>");
  file1.println("</tr>");
  file1.println("</table>");

  file1.println("<br>");

  file1.println("<br>");
  file1.println("<div style=\"display:flex; justify-content:center;\">");
  file1.println("<button style=\"font-size: 40px; text-align:center; margin-right:20px; padding: 30px;\" onclick=\"location.href='/'\">Return to Display Page</button>");
  file1.println("</div>");

  file1.println("<script>");

  file1.println("setInterval(function() {");
  file1.println("var xhttp = new XMLHttpRequest();");
  file1.println("xhttp.onreadystatechange = function() {");
  file1.println("if (this.readyState == 4 && this.status == 200) {");
  file1.println("document.getElementById(\"rktStatus\").innerHTML = this.responseText;");
  file1.println("}");
  file1.println("};");
  file1.println("xhttp.open(\"GET\", \"/sys\", true);");
  file1.println("xhttp.send();");
  file1.println("}, 1000);");

  file1.println("function setRktState(on) {");
  file1.println("var xhr = new XMLHttpRequest();");
  file1.println("xhr.open('POST', '/rkt', true);");
  file1.println("xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');");
  file1.println("xhr.onreadystatechange = function() {");
  file1.println("if (xhr.readyState === 4 && xhr.status === 200) {");
  file1.println("var response = JSON.parse(xhr.responseText);");
  file1.println("document.getElementById('rktStatus').innerHTML = response.stringRkt;");
  //file1.println("document.getElementById('on-rkt').disabled = response.rktState;");
  //file1.println("document.getElementById('off-rkt').disabled = !response.rktState;");
  file1.println("}");
  file1.println("};");
  file1.println("xhr.send('rktStatus=' + (on ? 'on' : 'off'));");
  file1.println("}");

  file1.println("function submitPHOffSet() {");
  file1.println("var xhr = new XMLHttpRequest();");
  file1.println("xhr.open(\"POST\", \"/phoff\");");
  file1.println("xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");");
  file1.println("var phnum = document.getElementById(\"phvalue\").value;");
  file1.println("var phdata = \"phnum=\" + encodeURIComponent(phnum);");
  file1.println("xhr.send(phdata);");
  file1.println("document.getElementById(\"phform\").reset();");
  file1.println("alert(\"pH Value Recalibrated!\");");
  file1.println("return false;");
  file1.println("}");

  file1.println("function submitNewVoltage() {");
  file1.println("var xhr = new XMLHttpRequest();");
  file1.println("xhr.open(\"POST\", \"/volt\");");
  file1.println("xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");");
  file1.println("var voltnum = document.getElementById(\"voltvalue\").value;");
  file1.println("var voltdata = \"voltnum=\" + encodeURIComponent(voltnum);");
  file1.println("xhr.send(voltdata);");
  file1.println("document.getElementById(\"voltform\").reset();");
  file1.println("alert(\"Voltage Value Recalibrated!\");");
  file1.println("return false;");
  file1.println("}");

  file1.println("function submitNewTemp() {");
  file1.println("var xhr = new XMLHttpRequest();");
  file1.println("xhr.open(\"POST\", \"/do_temp\");");
  file1.println("xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");");
  file1.println("var tmpnum = document.getElementById(\"tmpvalue\").value;");
  file1.println("var tmpdata = \"tmpnum=\" + encodeURIComponent(tmpnum);");
  file1.println("xhr.send(tmpdata);");
  file1.println("document.getElementById(\"tmpform\").reset();");
  file1.println("alert(\"DO Temperature Value Recalibrated!\");");
  file1.println("return false;");
  file1.println("}");

  file1.println("function enterec() {");
  file1.println("fetch(\"/button1\");");
  file1.println("alert(\"Salinity Calibration Turned On!\");");
  file1.println("}");
  file1.println("function calcec() {");
  file1.println("fetch(\"/button2\");");
  file1.println("alert(\"Sets new Salinity Calibration!\");");
  file1.println("}");
  file1.println("function exitec() {");
  file1.println("fetch(\"/button3\");");
  file1.println("alert(\"Salinity Calibration Turned Off!\");");
  file1.println("}");

  file1.println("setInterval(function() {");
  file1.println("var xhttp = new XMLHttpRequest();");
  file1.println("xhttp.onreadystatechange = function() {");
  file1.println("if (this.readyState == 4 && this.status == 200) {");
  file1.println("document.getElementById(\"dslv_oxgn\").innerHTML = this.responseText;");
  file1.println("}");
  file1.println("};");
  file1.println("xhttp.open(\"GET\", \"/do_cal\", true);");
  file1.println("xhttp.send();");
  file1.println("}, 1000);");

  file1.println("setInterval(function() {");
  file1.println("var xhttp = new XMLHttpRequest();");
  file1.println("xhttp.onreadystatechange = function() {");
  file1.println("if (this.readyState == 4 && this.status == 200) {");
  file1.println("document.getElementById(\"saln\").innerHTML = this.responseText;");
  file1.println("}");
  file1.println("};");
  file1.println("xhttp.open(\"GET\", \"/saln_cal\", true);");
  file1.println("xhttp.send();");
  file1.println("}, 1000);");

  file1.println("setInterval(function() {");
  file1.println("var xhttp = new XMLHttpRequest();");
  file1.println("xhttp.onreadystatechange = function() {");
  file1.println("if (this.readyState == 4 && this.status == 200) {");
  file1.println("document.getElementById(\"ph_lvl\").innerHTML = this.responseText;");
  file1.println("}");
  file1.println("};");
  file1.println("xhttp.open(\"GET\", \"/ph_cal\", true);");
  file1.println("xhttp.send();");
  file1.println("}, 1000);");

  file1.println("setInterval(function() {");
  file1.println("var xhttp = new XMLHttpRequest();");
  file1.println("xhttp.onreadystatechange = function() {");
  file1.println("if (this.readyState == 4 && this.status == 200) {");
  file1.println("document.getElementById(\"wtr_temp\").innerHTML = this.responseText;");
  file1.println("}");
  file1.println("};");
  file1.println("xhttp.open(\"GET\", \"/temp_cal\", true);");
  file1.println("xhttp.send();");
  file1.println("}, 1000);");

  file1.println("</script>");

  file1.println("</body>");
  file1.println("</html>");

  file1.close();

  Serial.println("File1 created");

  File file = SPIFFS.open("/index.html", "w");
  if (!file) {
    Serial.println("Failed to create file");
    return;
  }

  file.println("<!DOCTYPE html>");
  file.println("<html>");
  file.println("<head>");

  file.println("<title>REKTfier</title>");

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
  file.println("<h1 style=\"font-size: 65px; text-align:center\">Water Monitoring System</h1>");

  file.println("<br>");

  file.println("<h2>These are the parameters that must be maintain to achieve a good quality water:</h2>");
  file.println("<ul>");
  file.println("<li><h2>Ideal Dissolve Oxygen: Above 4.00mg/L</h2></li>");
  file.println("<li><h2>Ideal Salinity Level: Between 15.00ppt and 25.00ppt</h2></li>");
  file.println("<li><h2>Ideal pH Level: Between 6.50pH and 9.00pH</h2></li>");
  file.println("<li><h2>Ideal Water Temperature: Between 25</span><span>&#176;</span>C and 31</span><span>&#176;</span>C</h2></li>");
  file.println("</ul>");

  file.println("<br><br>");

  file.println("<table style=\"width: 100%\"");
  file.println("<tr>");
  file.println("<th>Parameters</th>");
  file.println("<th>Readings</th>");
  file.println("<th>Remarks</th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Dissolved Oxygen: </th>");
  file.println("<th><span id=\"dslv_oxgn\">Loading...</span>mg/L</th>");
  file.println("<th><span id=\"do_findings\">Loading...</span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Salinity Level: </th>");
  file.println("<th><span id=\"saln\">Loading...</span>PPT</th>");
  file.println("<th><span id=\"saln_findings\">Loading...</span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>pH Level: </th>");
  file.println("<th><span id=\"ph_lvl\">Loading...</span>pH</th>");
  file.println("<th><span id=\"ph_findings\">Loading...</span></th>");
  file.println("</tr>");

  file.println("<tr>");
  file.println("<th>Temperature: </th>");
  file.println("<th><span id=\"wtr_temp\">Loading...</span><span>&#176;</span>C</th>");
  file.println("<th><span id=\"temp_findings\">Loading...</span></th>");
  file.println("</tr>");
  file.println("</table>");

  file.println("<br>");

  file.println("<h3 style=\"font-size:35px;\">Motor Status: <span id=\"mtr_status\">Loading...</h3>");

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
  file.println("}, 1000);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"saln\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/saln_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"ph_lvl\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/ph_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  file.println("document.getElementById(\"wtr_temp\").innerHTML = this.responseText;");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/temp_sensor\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

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
  file.println("}, 1000);");

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
  file.println("}, 1000);");

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
  file.println("}, 1000);");

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
  file.println("}, 1000);");

  file.println("const mtrElement = document.getElementById(\"mtr_status\");");
  file.println("setInterval(function() {");
  file.println("var xhttp = new XMLHttpRequest();");
  file.println("xhttp.onreadystatechange = function() {");
  file.println("if (this.readyState == 4 && this.status == 200) {");
  //  file.println("document.getElementById(\"mtr_status\").innerHTML = this.responseText;");
  file.println("const mtr_val = this.responseText;");
  file.println("mtrElement.innerHTML = mtr_val;");
  file.println("mtrElement.style.color = mtr_val === \"ON\" ? \"green\" : \"red\";");
  file.println("}");
  file.println("};");
  file.println("xhttp.open(\"GET\", \"/mtr_state\", true);");
  file.println("xhttp.send();");
  file.println("}, 1000);");

  file.println("</script>");

  file.println("</body>");
  file.println("</html>");

  file.close();

  Serial.println("File created");
}

void loop() {
  // Empty loop
}
