#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>

#define screen_width 128
#define screen_height 64
#define buzzer D8

// WiFi card example
char ssid[] = "Sentrify Technologies Corp. 2.4G";         // your SSID
char pass[] = "STC2022!";     // your SSID Password
char server[] = "192.168.100.7"; // type ipconfig in cmd and enter the value of IPv4 address here
float temp_adjust = 2; //This is where you put adjustment for precision
float minthreshold_temp = 33;
float maxthreshold_temp = 60;


WiFiClient client;                 // Use this for WiFi instead of EthernetClient
Adafruit_SSD1306 display(screen_width, screen_height);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float last_value = 0;
float temperature = 0;
void setup()
{ 
  Serial.begin(115200);

  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  // display

  delay(1000);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  mlx.begin();
  pinMode(buzzer, OUTPUT); 
}
 
void loop()
{
  delay(500);
  temperature = (mlx.readObjectTempC() + temp_adjust);
  Serial.println(temperature);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("Not turned on");
  }

  if(temperature > minthreshold_temp && temperature < maxthreshold_temp){
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    last_value = temperature;
    delay(500);
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,20);
    display.print(last_value);
    display.display();
    Sending_To_phpmyadmindatabase();

    
  }
}

void Sending_To_phpmyadmindatabase()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /thermalscanner/scanner.php?value=");
    client.print("GET /thermalscanner/scanner.php?value=");     //YOUR URL
    Serial.println(temperature);
    client.print(temperature);
    client.print("&code=");
    Serial.println("&code=");
    client.print(1);
    Serial.println(1);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("192.168.100.7");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }
