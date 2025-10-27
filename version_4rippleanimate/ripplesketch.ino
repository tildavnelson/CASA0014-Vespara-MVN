// Duncan Wilson Oct 2025 - v1 - MQTT messager to vespera
// works with MKR1010

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>  // for RGB LED on MKR1010

/*
**** enter sensitive data in arduino_secrets.h:
#define SECRET_SSID "your-ssid"
#define SECRET_PASS "your-password"
#define SECRET_MQTTUSER "mqtt-username"
#define SECRET_MQTTPASS "mqtt-password"
*/

// WiFi and MQTT credentials
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

// Create WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// VESPARA CONFIG
String lightId = "19";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId = "vespMVN";

// NeoPixel Configuration
const int num_leds = 72;
const int payload_size = num_leds * 3; // RGB per pixel
byte RGBpayload[payload_size];
#define NEOPIXEL_PIN 2  // change if your NeoPixel data line uses another pin
Adafruit_NeoPixel strip(num_leds, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


// HC-SR04 Ultrasonic Sensor

const int trigPin = 7;
const int echoPin = 6;
float duration, distance;

void setup() {
  Serial.begin(9600);
  //while (!Serial)
    
  Serial.println("Vespera");
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    printMacAddress(mac);
    Serial.print("This device is Vespera ");
    Serial.println(lightId);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    startWifi();

    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setBufferSize(2000);
    mqttClient.setCallback(callback);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Serial.println("Set-up complete");
}


void loop() {
  // ULTRASONIC SENSOR, SENDING SOUND, RECEIVING SOUND AND MEASURING THE DISTANCE
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);

// MAINTAIN MQTT and WIFI CONNECTIONS
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // DISTANCE BASED COLOUR
  int r, g, b;
  setColorFromDistance(distance, r, g, b);

  //RIPPLE ANIMATION STARTING FROM GROUP 1 TO 7
  Ripleout(r, g, b, 150);
  delay(200); 

  }


// MATCH DISTANCE TO COLOUR
void setColorFromDistance(float distance, int &r, int &g, int &b) {
  if (distance < 20) {
    r = 53; g = 3; b = 252;   // blue
  } else if (distance < 40) {
    r = 3; g = 12; b = 252;   // greenyblue
  } else if (distance < 60) {
    r = 0; g = 184; b = 194;  // green
  } else if (distance < 80) {
    r = 0; g = 173; b = 92;   // greenyyellow
  } else if (distance < 100) {
    r = 177; g = 212; b = 2;  // pale greenyyellow
  } else if (distance < 120) {
    r = 252; g = 236; b = 3;  // yellow
  } else if (distance < 140) {
    r = 252; g = 169; b = 3;  // orangy yellow
  } else if (distance < 160) {
    r = 252; g = 30; b = 3;   // orange
  } else if (distance < 180) {
    r = 252; g = 3; b = 102;  // red
  } else if (distance < 200) {
    r = 252; g = 3; b = 244;  // pink
    } else {
    r = 0; g = 0; b = 0;
  }
}

// Print MAC address to serial
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0) Serial.print(":");
  }
  Serial.println();
}

// MQTT message handler (stubbed)
void callback(char* topic, byte* payload, unsigned int length) {
  // No-op or future message handling
}

// WiFi reconnect logic
void startWifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("\nWiFi connected.");
}

// MQTT reconnect logic
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String cid = "client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(cid.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected.");
      mqttClient.subscribe(mqtt_topic.c_str()); // Optional
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Send color to individual pixel via MQTT
//void send_RGB_to_pixel(int r, int g, int b, int pixel) {
 //if (pixel >= 0 && pixel < num_leds) {
    //RGBpayload[pixel * 3 + 0] = r;
    //RGBpayload[pixel * 3 + 1] = g;
   // RGBpayload[pixel * 3 + 2] = b;

   // strip.setPixelColor(pixel, strip.Color(r, g, b));
   // strip.show();

    //mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  

void lightGroup(int leds[], int count, int r, int g, int b) {
  for (int i = 0; i < count; i++) {
    int pixel = leds[i];
    if (pixel >= 0 && pixel < num_leds) {
      strip.setPixelColor(pixel, strip.Color(r, g, b));
      RGBpayload[pixel * 3 + 0] = r;
      RGBpayload[pixel * 3 + 1] = g;
      RGBpayload[pixel * 3 + 2] = b;
    }
  }
  strip.show();
  mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
}

//GROUPS OF LIGHTS THAT LIGHTS TOGETHER AND LIGHT UP IN SEQUENCE
void Ripleout(int r, int g, int b, int stepDelay) { strip.clear();
  int group1[] = {46};
  lightGroup(group1, sizeof(group1) / sizeof(group1[0]), r, g, b);
  delay(stepDelay);

  int group2[] = {39,45,51,52,53,47,41,40};
  lightGroup(group2, sizeof(group2) / sizeof(group2[0]), r, g, b);
  delay(stepDelay);

  int group3[] = {17,23,29,35,34,33,32,38,44,50,56,57,58,59,65};
  lightGroup(group3, sizeof(group3) / sizeof(group3[0]), r, g, b);
  delay(stepDelay);

  int group4[] = {5,11,16,22,28,27,26,25,31,37,43,49,55,61,62,63,64,71};
  lightGroup(group4, sizeof(group4) / sizeof(group4[0]), r, g, b);
  delay(stepDelay);

  int group5[] = {4,10,15,21,20,19,18,24,30,36,42,48,54,60,66,67,68,69,70};
  lightGroup(group5, sizeof(group5) / sizeof(group5[0]), r, g, b);
  delay(stepDelay);

  int group6[] = {0,1,2,3,9,14,13,12};
  lightGroup(group6, sizeof(group6) / sizeof(group6[0]), r, g, b);
  delay(stepDelay);

  int group7[] = {6,7,8};
  lightGroup(group7, sizeof(group7) / sizeof(group7[0]), r, g, b);
  delay(stepDelay);

}
