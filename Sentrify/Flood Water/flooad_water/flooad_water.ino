//pH Level Sensor
#define phLvl A0

//Hand & Foot Sensor
#define ir_sense1 2
#define ir_sense2 3

//Motor Switch
#define motor 12

//Float Sensor
#define float1 6
#define float2 7

//pH Sensor Variables
float temp_ph_value = 0;
float ph_value = 0;
float volt = 0;
float ph_act = 0;
float calibration_value = 22.69 - 1.57;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

//ORP Sensor Variables
float orp = 0;

//IR Sensor Variables
bool ir1 = 0;
bool ir2 = 0;

//Floter Sensor Variables
int read_float1 = 0;
int read_float2 = 0;

//Counter Variables
int counter = 0;

//IO Variables
int input = 0;
bool swtch = false;

//String Variables
String byte_send = "";
String v1 = "";
String v2 = "";
String wtr_lvl = "";

//Delay Variables
unsigned long current_time = 0;
unsigned long prev_time = 0;
unsigned long delay_time = 1000;
unsigned long prev_time_ph = 0;
unsigned long delay_time_ph = 1000;


void setup() {
  Serial.begin(9600);  // Serial monitoring

  pinMode(ir_sense1, INPUT);
  pinMode(ir_sense2, INPUT);

  pinMode(float1, INPUT_PULLUP);
  pinMode(float2, INPUT_PULLUP);
}

//Function Declarations
float pH_sensor();
void read_info();
void send_bluetooth();

void loop() {
  current_time = millis();

  ir1 = digitalRead(ir_sense1);
  ir2 = digitalRead(ir_sense2);

  read_float1 = digitalRead(float1);
  read_float2 = digitalRead(float2);

  read_info();

  temp_ph_value = pH_sensor();

  if (v1 == "Close" && v2 == "Close") {
    if (current_time - prev_time_ph >= delay_time_ph) {
      prev_time_ph = current_time;
      counter++;
      if (counter >= 20) {
        ph_value = temp_ph_value;
      }
    }
  } else {
    counter = 0;
  }

  send_bluetooth();
}

void read_info() {

  v1 = (ir1 == false) ? "Open" : "Close";
  v2 = (ir2 == false) ? "Open" : "Close";

  if (read_float1 == LOW && read_float2 == LOW) {
    wtr_lvl = "< 70";
  }

  else if (read_float1 == HIGH && read_float2 == LOW) {
    wtr_lvl = "70";
  }

  else if (read_float1 == HIGH && read_float2 == HIGH) {
    wtr_lvl = "100";
  }

  if (swtch == true) {
    if (wtr_lvl == "< 70" || wtr_lvl == "70") {
      digitalWrite(motor, HIGH);
    }
    else if (wtr_lvl == "100") {
      digitalWrite(motor, LOW);
    }
  }

  else if (swtch == false) {
    digitalWrite(motor, LOW);
  }
}

float pH_sensor() {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(phLvl);
    delay(10);
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
  volt = (float)avgval * 5.0 / 1023.0 / 6;
  ph_act = -5.70 * volt + calibration_value;
  return ph_act;
}

void send_bluetooth() {
  if (Serial.available() > 0 || current_time - prev_time >= delay_time) {
    prev_time = current_time;
    byte_send = (String)ph_value + "," + (String)orp + "," + v1 + "," + v2 + "," + wtr_lvl;
    Serial.println(byte_send);
    input = Serial.read();
    if (input == '1') {
      swtch = true;
    } else if (input == '2') {
      swtch = false;
    }
  }
}