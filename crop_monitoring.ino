
// Include the library files
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>

const int thresholdDistance = 50;// threshold distance for ultrasonic sensor
const char* server = "api.thingspeak.com";//thigspeak server
const String apiKey = "BLTD065NSX2D2EG9";//thingspeak apikey
const int analogInPin = A0;  // Analog input pin that the soil moisture sensor is connected to

// Define the motor pins
#define ENA 11
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 8
#define ENB 12

#define TRIGGER_PIN 14 // trigger pin of ultrasonic sensor 
#define ECHO_PIN 16 //echo pin of ultrasonic sensor
#define DHTPIN 15//data pin of dht11
#define DHTTYPE DHT11
// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);
// Variables for the Blynk widget values
int x = 50;
int y = 50;
int speed;

Servo servo;

char auth[] = "GdhSbGKHhjud4lBKaxmRRGYAx3Pg__5q"; //Enter your Blynk auth token
char ssid[] = "name"; //Enter your WIFI name
char pass[] = "pass"; //Enter your WIFI passowrd


void setup() {
  Serial.begin(9600);
//Set the motor pins as output pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

// Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(TRIGGER_PIN, OUTPUT);// Set trigger pin as output
  digitalWrite(TRIGGER_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);// Set echo pin as input
  
  dht.begin();
  servo.attach(9);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);  // Initialize the Blynk library
}

// Get the joystick values
BLYNK_WRITE(V0) {
  x = param[0].asInt();
}
// Get the joystick values
BLYNK_WRITE(V1) {
  y = param[0].asInt();
}
//Get the slider values 
BLYNK_WRITE(V2) {
  speed = param.asInt();
}
//Get the slider values 
BLYNK_WRITE(V3){
  servo.write(param.asInt());
}
// Check these values using the IF condition
void smartcar() {
  if (y > 70) {
    carForward();
    Serial.println("carForward");
  } else if (y < 30) {
    carBackward();
    Serial.println("carBackward");
  } else if (x < 30) {
    carLeft();
    Serial.println("carLeft");
  } else if (x > 70) {
    carRight();
    Serial.println("carRight");
  } else if (x < 70 && x > 30 && y < 70 && y > 30) {
    carStop();
    Serial.println("carstop");
  }
}
void loop() {
  Blynk.run();// Run the blynk function
  smartcar();// Call the main function

// Read soil moisture sensor data
  int sensorValue = analogRead(analogInPin);
  Serial.print("sensorValue");
  Serial.println(sensorValue);
//Read dht11 data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.print("humidity");
  Serial.println(humidity);
  Serial.print("temperature");
  Serial.println(temperature);
//Read ultrasonic sensor data
  float duration, distance;
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("distance");
  Serial.println(distance);
  
// Send data to ThingSpeak
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(distance);
    postStr += "\r\n\r\n";
    postStr += "&field2=";
    postStr += String(sensorValue);
    postStr += "\r\n\r\n";
    postStr += "&field3=";
    postStr += String(temperature);
    postStr += "\r\n\r\n";
    postStr += "&field4=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";

  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);

  Serial.println("Data sent to ThingSpeak");
  //delay(3000);
}

/************Motor movement functions***************/
void carBackward() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carForward() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carRight() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carLeft() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}