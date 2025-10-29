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


// HC-SR04 UlTRASONIC SENSOR
const int trigPin = 7;
const int echoPin = 6;
float duration, distance;


int lastR = 0, lastG = 0, lastB = 255;  // KEEPS THE LAST COLOUR
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 300; // TIME BETWEEN NEW READINGS (ms)

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

    // Store last colour so it stays until next reading
int lastR = 0;
int lastG = 0;
int lastB = 255;  // start blue
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
  // ONLY run the ripple if the colour actually changed
  if (r != lastR || g != lastG || b != lastB) {
    Ripleout(r, g, b, 150);
    lastR = r;
    lastG = g;
    lastB = b;
  } else {
    // KEEP THE LAST COLOUR VISIBLE
    for (int i = 0; i < num_leds; i++) {
      strip.setPixelColor(i, strip.Color(lastR, lastG, lastB));
    }
    strip.show();
  }

  delay(200);  

  }


// MATCH DISTANCE TO COLOUR
void setColorFromDistance(float distance, int &r, int &g, int &b) {
 if (distance < 10) {
    r = 0;   g = 0;   b = 255;    // DEEP BLUE
  } else if (distance < 20) {
    r = 0;   g = 60;  b = 255;    // MEDIUM BLUE
  } else if (distance < 30) {
    r = 0;   g = 120; b = 255;    // SKY BLUE
  } else if (distance < 40) {
    r = 0;   g = 180; b = 255;    // CYAN
  } else if (distance < 50) {
    r = 0;   g = 240; b = 200;    // CYAN-GREEN
  } else if (distance < 60) {
    r = 0;   g = 255; b = 120;    // GREEN-TURQUOISE
  } else if (distance < 70) {
    r = 60;  g = 255; b = 60;     // BRIGHT GREEN
  } else if (distance < 80) {
    r = 140; g = 255; b = 0;      // LIME GREEN
  } else if (distance < 90) {
    r = 200; g = 255; b = 0;      // YELLOW-GREEN
  } else if (distance < 100) {
    r = 255; g = 255; b = 0;      // YELLOW
  } else if (distance < 110) {
    r = 255; g = 200; b = 0;      // AMBER-YELLOW
  } else if (distance < 120) {
    r = 255; g = 140; b = 0;      // ORANGE
  } else if (distance < 130) {
    r = 255; g = 80;  b = 0;      // DEEP ORANGE
  } else if (distance < 140) {
    r = 255; g = 0;   b = 0;      // RED
  } else if (distance < 150) {
    r = 255; g = 0;   b = 80;     // RED-PINK
  } else if (distance < 160) {
    r = 255; g = 0;   b = 150;    // PINK
  } else if (distance < 170) {
    r = 255; g = 0;   b = 200;    // HOT PINK
  } else if (distance < 180) {
    r = 255; g = 40;  b = 255;    // MAGENTA
  } else if (distance < 190) {
    r = 230; g = 80;  b = 255;    // PURPLE-PINK
  } else { // 190–200+
    r = 200; g = 100; b = 255;    // VIOLET
  }
}
//MORE STABLE READINGS
  float getStableDistance() {
  const int samples = 5;  // take 5 readings
  float total = 0;}

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
void Ripleout(int r, int g, int b, int stepDelay) {;
  int group1[] = {5,17,29,41,53,65};
  lightGroup(group1, sizeof(group1) / sizeof(group1[0]), r, g, b);
  delay(stepDelay);

  int group2[] = {4,11,16,23,28,35,40,47,52,59,64,71};
  lightGroup(group2, sizeof(group2) / sizeof(group2[0]), r, g, b);
  delay(stepDelay);

  int group3[] = {3,10,15,22,27,34,39,46,51,58,63,70};
  lightGroup(group3, sizeof(group3) / sizeof(group3[0]), r, g, b);
  delay(stepDelay);

  int group4[] = {2,9,14,21,26,33,38,45,50,57,62,69};
  lightGroup(group4, sizeof(group4) / sizeof(group4[0]), r, g, b);
  delay(stepDelay);

  int group5[] = {1,8,13,20,25,32,37,44,49,56,61,68};
  lightGroup(group5, sizeof(group5) / sizeof(group5[0]), r, g, b);
  delay(stepDelay);

  int group6[] = {0,7,12,19,24,31,36,43,48,55,60,67};
  lightGroup(group6, sizeof(group6) / sizeof(group6[0]), r, g, b);
  delay(stepDelay);

  int group7[] = {6,18,30,42,54,66};
  lightGroup(group7, sizeof(group7) / sizeof(group7[0]), r, g, b);
  delay(stepDelay);

}
