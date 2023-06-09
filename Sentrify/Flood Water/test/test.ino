/*
Arduino Turn LED On/Off using Serial Commands
Created April 22, 2015
Hammad Tariq, Incubator (Pakistan)

It's a simple sketch which waits for a character on serial
and in case of a desirable character, it turns an LED on/off.

Possible string values:
a (to turn the LED on)
b (tor turn the LED off)
*/

char junk;
String inputString = "";
int read = 0;
int input;

unsigned long current_time = 0;
unsigned long prev_time = 0;
unsigned long delay_time = 500;

void setup()  // run once, when the sketch starts
{
  Serial.begin(9600);  // set the baud rate to 9600, same should be of your Serial Monitor
  pinMode(2, OUTPUT);
}

void loop() {
  current_time = millis();
  if (Serial.available() > 0 || current_time - prev_time >= delay_time) {
    prev_time = current_time;
    input = Serial.read();
    if (input == '1') {  //in case of 'a' turn the LED on
      digitalWrite(2, HIGH);
    } else if (input == '2') {  //incase of 'b' turn the LED off
      digitalWrite(2, LOW);
    }
    inputString = "";
    Serial.println(read);
  }
  read = analogRead(A0);
}
