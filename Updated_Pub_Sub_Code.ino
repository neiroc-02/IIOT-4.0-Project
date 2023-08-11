/*Rui Santos - Complete project details at https://randomnerdtutorials.com Updated by Neiro Cabrera*/
#include <WiFi.h>
#include <PubSubClient.h>  //By Nick O'Leary see:https://github.com/knolleary/pubsubclient and http://www.steves-internet-guide.com/using-arduino-pubsub-mqtt-client/
#include <Wire.h>
#include "SSD1306Wire.h"  //By ThingPulse see:https://github.com/ThingPulse/esp8266-oled-ssd1306
#include <Stepper.h>      //Default Arduino library

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

//Inputs
#define IN1_pin 26
#define IN2_pin 27
#define IN3_pin 14
#define IN4_pin 12
#define EXTRA_IO_pin 13
#define sensor1 A4

//Ultrasonic Sensor 
#define TRIG_pin 25
#define ECHO_pin 33

//Potentiometer
#define POTENTIOMETER_pin 33

//Input Pins for Pushbuttons and Switches
#define ON_SW_pin 35
#define AUTO_SW_pin 34
#define START_PB_pin 39
#define STOP_PB_pin 36
#define USR_PB_pin 0

//Output Pins
#define OUT0_pin 5
#define OUT1_pin 16
#define OUT2_pin 15
#define OUT3_pin 2

//Stepper Motor Pins
#define STEPPER_pin1 4
#define STEPPER_pin2 18
#define STEPPER_pin3 17
#define STEPPER_pin4 19

//Display Pins
#define OLED_SDA_pin 22  //OLED SDA of ESP32
#define OLED_SCL_pin 21  //OLED SCL of ESP32
#define OLED_RST 16      //Optional, TTGO board contains OLED_RST connected to pin 16 of ESP32
#define OLED_ADDR 0x3c
SSD1306Wire display(OLED_ADDR, OLED_SDA_pin, OLED_SCL_pin);

// Replace the next variables with your SSID/Password combination
const char* ssid = "IT4Project";
const char* password = "IOT12345";
const char* mqtt_server = "192.168.10.94";  //Right IP as long as Laptop is connected to Router at School
const char* client_name = "line1/thing19";  //Each client needs to be different for one broker
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// initialize the stepper library
Stepper myStepper(stepsPerRevolution, STEPPER_pin1, STEPPER_pin2, STEPPER_pin3, STEPPER_pin4);
//Stepper myStepper(200, STEPPER_pin1, STEPPER_pin2, STEPPER_pin3, STEPPER_pin4);

//Initialize Namespace
char client_topic[50];
//char client_temperature[50];
char client_potentiometer[50];
char client_count[50];
char client_sensor1[50];
char client_sensor2[50];
char client_digitalIn[50];
char client_message[50];
char client_out1[50];
char client_out2[50];
char client_out3[50];
char client_out4[50];
char client_stepperStep[50];
char client_stepperSpeed[50];
char client_subscribe_all[50];

WiFiClient esp32Client;
PubSubClient client(esp32Client);

int speed = 5;

long lastMsg = 0;

char msg[50];
int value = 0;

int sensorValue = 0;
int sensorValueOld = -1;
String mes = "0";
String meas = "0";

float duration_us, distance_cm, distance_cm_old = -1;
int temprature = 0;
int old_temprature = -1;
int potentiometer = 0;
int old_potentiometer = -1;

String inputsNew = "test";
String inputsOld = "test2";

void setup() {
  //set pins
  pinMode(IN1_pin, INPUT_PULLUP);
  pinMode(IN2_pin, INPUT_PULLUP);
  pinMode(IN3_pin, INPUT_PULLUP);
  pinMode(IN4_pin, INPUT_PULLUP);
  pinMode(EXTRA_IO_pin, INPUT);
  pinMode(TRIG_pin, OUTPUT);
  pinMode(ECHO_pin, INPUT);
  pinMode(POTENTIOMETER_pin, INPUT);
  pinMode(ON_SW_pin, INPUT_PULLUP);
  pinMode(AUTO_SW_pin, INPUT_PULLUP);
  pinMode(START_PB_pin, INPUT);
  pinMode(STOP_PB_pin, INPUT);
  pinMode(USR_PB_pin, INPUT);

  pinMode(OUT0_pin, OUTPUT);
  pinMode(OUT1_pin, OUTPUT);
  pinMode(OUT2_pin, OUTPUT);
  pinMode(OUT3_pin, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //Declare fount for OLED
  display.setFont(ArialMT_Plain_10);

  //setup namespace
  //sprintf(client_temperature, "%s/temp", client_name);
  sprintf(client_potentiometer, "%s/in/pot", client_name);
  sprintf(client_count, "%s/count", client_name);
  sprintf(client_sensor1, "%s/in/sensor1", client_name);
  sprintf(client_sensor2, "%s/in/sensor2", client_name);
  sprintf(client_message, "%s/out/message", client_name);
  sprintf(client_out1, "%s/out/out1", client_name);
  sprintf(client_out2, "%s/out/out2", client_name);
  sprintf(client_out3, "%s/out/out3", client_name);
  sprintf(client_out4, "%s/out/out4", client_name);
  sprintf(client_stepperStep, "%s/out/stepperStep", client_name);
  sprintf(client_stepperSpeed, "%s/out/stepperSpeed", client_name);
  sprintf(client_digitalIn, "%s/in/digitalIn", client_name);
  sprintf(client_subscribe_all, "%s/out/#", client_name);
}

void setup_wifi() {
  delay(10);
  display.init();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, String(client_name));
  display.drawString(0, 16, "Connecting to...");
  display.drawString(0, 32, String(ssid));
  display.display();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //IMPORTANT 
  IPAddress staticIP(192, 168, 10, 119); //NOTE: CHANGE THIS FOR EVERY DEVICE!
  IPAddress gateway(192, 168, 10, 1);
  IPAddress subnet(255, 255, 255, 0);
  //IMPORTANT

  WiFi.config(staticIP, gateway, subnet);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(1000);
  display.init();
  display.drawString(0, 0, "Wifi Connected!");
  display.drawString(0, 16, "Local IP address:");
  display.drawString(0, 32, WiFi.localIP().toString().c_str());
  display.display();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

//This function reads subscritptions
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println("Message arrived!!!");
  //Serial.print(client_message);

  Serial.print(topic);
  Serial.print(": ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  display.setColor(BLACK);
  display.fillRect(0, 45, 127, 64);
  display.setColor(WHITE);  //clear display area
  //display.drawString(0, 45, client_message);
  display.drawString(0, 45, topic);
  display.drawString(0, 54, messageTemp);
  display.display();
  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  /*if (String(topic) == "client_topic/message") { //Subscribed to hello/sensor
    Serial.print("Recieving Data...");
    delay(500);
    if(messageTemp){ //Syntax if I wanna do something when a message appears
    }
    else{
    }
    }*/
if (String(topic) == client_stepperSpeed) speed=messageTemp.toInt();
if (String(topic) == client_stepperStep) {
    //set outputs because stepper motor heats up in idle
    pinMode(STEPPER_pin1, OUTPUT);
    pinMode(STEPPER_pin2, OUTPUT);
    pinMode(STEPPER_pin3, OUTPUT);
    pinMode(STEPPER_pin4, OUTPUT);
        int step = messageTemp.toInt();
    Serial.println(step);
    //int sensorReading = analogRead(A4);
    // map it to a range from 0 to 100:
    //int motorSpeed = map(sensorReading, 0, 4095, 0, 30);
    //myStepper.setSpeed(motorSpeed);
    myStepper.setSpeed(speed);
    myStepper.step(step);
    //turn off outputs because stepper motor heats up in idle
    pinMode(STEPPER_pin1, INPUT_PULLUP);
    pinMode(STEPPER_pin2, INPUT_PULLUP);
    pinMode(STEPPER_pin3, INPUT_PULLUP);
    pinMode(STEPPER_pin4, INPUT_PULLUP);
}
  if (messageTemp == "True" || messageTemp == "1") {
    messageTemp = "1";
  } else if (messageTemp == "False" || messageTemp == "0") {
    messageTemp = "0";
  }
  if (String(topic) == client_out1) {
    digitalWrite(OUT0_pin, messageTemp.toInt());
  }
  if (String(topic) == client_out2) {
    digitalWrite(OUT1_pin, messageTemp.toInt());
  }
  if (String(topic) == client_out3) {
    digitalWrite(OUT2_pin, messageTemp.toInt());
  }
  if (String(topic) == client_out4) {
    digitalWrite(OUT3_pin, messageTemp.toInt());
  }
}

void reconnect() {
  display.init();                                   //clear display when connected
  while (!client.connected()) {                     // Loop until reconnected
    Serial.print("Attempting MQTT connection...");  // Attempt to connect
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Attempting MQTT");
    display.drawString(0, 16, "connection with:");
    display.drawString(0, 32, String(mqtt_server));
    display.display();
    if (client.connect(client_name)) {  //Client Name must be unique for every device in the network
      Serial.println("connected");
      display.drawString(0, 48, "Reconnected!!!");
      display.display();
      // Subscribe
      client.publish(client_message, "Reconnected!");
      //client.subscribe(client_message);
      client.subscribe(client_subscribe_all);  //read all /out/#

    } else {
      Serial.print("failed, rc=");
      display.drawString(0, 48, "Failed! Retrying...");
      display.display();
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");  // Wait 5 seconds before retrying
      delay(5000);
      display.init();  //clear display
    }
  }
  display.init();  //clear display when connected
  display.setFont(ArialMT_Plain_10);
}

void loop() {
  String message = "0";
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {

    //display.drawString(0, 0, client_topic);  //print topic if desired
    //Serial.println(client_topic); //print topic if desired
    /*
    //temprature
    temprature = temprature_sens_read();  // Convert raw temperature in F to Celsius degrees(temprature_sens_read() - 32) / 1.8;
    if (old_temprature != temprature) {
      message = String(old_temprature);  //
      display.setColor(BLACK);
      display.drawString(80, 0, message);
      display.setColor(WHITE);  //clear old display value
      message = String(temprature);
      client.publish(client_temperature, (char*)message.c_str());
      //display.drawString(0, 0, client_temperature); display.setTextAlignment(TEXT_ALIGN_RIGHT); display.drawString(80, 0, message); display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, client_temperature);
      display.drawString(80, 0, message);
      Serial.print(client_temperature);
      Serial.print(": ");
      Serial.print(message);
      Serial.println(" F");
      old_temprature = temprature;
    }
*/

    //count
    message = value;
    display.setColor(BLACK);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127, 0, message);
    display.setColor(WHITE);  //clear old display value
    lastMsg = now;
    ++value;
    message = value;
    client.publish(client_count, (char*)message.c_str());
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, client_count);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127, 0, message);
    Serial.print(client_count);
    Serial.print(": ");
    Serial.println(message);

    //sensor1 potentiometer
    sensorValue = analogRead(sensor1);
    if (sensorValueOld != sensorValue) {
      message = String(sensorValueOld);
      display.setColor(BLACK);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 10, message);
      display.setColor(WHITE);  //clear old display value
      message = String(sensorValue);
      client.publish(client_sensor1, (char*)message.c_str());
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      //display.drawString(0, 10, client_sensor1);
      display.drawString(0, 10, ".../sensor1");
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 10, message);
      Serial.print(client_sensor1);
      Serial.print(": ");
      Serial.println(message);
      sensorValueOld = sensorValue;
    }

    //sensor2 ultrasonic sensor
    digitalWrite(TRIG_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_pin, LOW);
    duration_us = pulseIn(ECHO_pin, HIGH);
    distance_cm = 0.017 * duration_us;
    if (distance_cm_old != distance_cm) {
      message = String(distance_cm_old);
      display.setColor(BLACK);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 20, message);
      display.setColor(WHITE);  //clear old display value
      message = String(distance_cm);
      client.publish(client_sensor2, (char*)message.c_str());
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      //display.drawString(0, 20, client_sensor2);
      display.drawString(0, 20, ".../sensor2");
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 20, message);
      Serial.print(client_sensor1);
      Serial.print(": ");
      Serial.println(message);
      distance_cm_old = distance_cm;
    }
    display.display();
  }

  //read Input Pin
  inputsNew = digitalRead(IN1_pin);
  inputsNew += digitalRead(IN2_pin);
  inputsNew += digitalRead(IN3_pin);
  inputsNew += digitalRead(IN4_pin);
  inputsNew += digitalRead(START_PB_pin);
  inputsNew += digitalRead(STOP_PB_pin);
  inputsNew += digitalRead(ON_SW_pin);
  inputsNew += digitalRead(AUTO_SW_pin);
  inputsNew += digitalRead(USR_PB_pin);

  if (inputsOld != inputsNew) {
    message = String(inputsOld);
    display.setColor(BLACK);
    //display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127, 30, message);
    display.setColor(WHITE);  //clear old display value
    message = String(inputsNew);
    client.publish(client_digitalIn, (char*)message.c_str());
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    //display.drawString(0, 30, client_digitalIn);
    display.drawString(0, 30, ".../digitalIn");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127, 30, message);
    Serial.print(client_digitalIn);
    Serial.print(": ");
    Serial.println(message);
    inputsOld = inputsNew;
    display.display();
  }
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}
