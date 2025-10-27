// Duncan Wilson Oct 2025 - v1 - MQTT messager to vespera
// works with MKR1010

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h" 
#include <utility/wifi_drv.h>   // library to drive to RGB LED on the MKR1010

/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below
#define SECRET_SSID "CE-Hub-student"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */
 //WIFI 
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* ssid1         = SECRET_SSID1;
const char* password1     = SECRET_PASS1;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

// create wifi object and mqtt object
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Make sure to update your lightid value below with the one you have been allocated
String lightId = "23"; // the topic id number or user number being used.

// Here we define the MQTT topic we will be publishing data to
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;            
String clientId = ""; // will set once i have mac address so that it is unique

// NeoPixel Configuration - we need to know this to know how to send messages 
// to vespera 
//LED CONFIGURATION
const int num_leds = 72;
const int payload_size = num_leds * 3; // x3 for RGB

// Create the byte array to send in MQTT payload this stores all the colours 
// in memory so that they can be accessed in for example the rainbow function
byte RGBpayload[payload_size];

// WHERE THE DATA IS GOING OUT AND COMING IN
const int trigPin = 7;  
const int echoPin = 6; 
float duration, distance;

void setup() {
  Serial.begin(115200);
  //while (!Serial); // Wait for serial port to connect (useful for debugging)
  Serial.println("Vespera");
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
  Serial.print("This device is Vespera ");
  Serial.println(lightId);
  // Connect to WiFi
  startWifi();
  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(2000);
  mqttClient.setCallback(callback);
  
  Serial.println("Set-up complete");


 // CODE FOR THE ULTRASONIC SENSOR
// * HC-SR04 example sketch
// * https://create.arduino.cc/projecthub/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-036380
//* by Isaac100
// setup code here, to run once:
	pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  
	Serial.begin(9600);
}
// Connect to WiFi
  startWifi();

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(2000);
  mqttClient.setCallback(callback);

void loop() 

// Sends out a sound wave, Waits for it to bounce back, Measures how long that took, Calculates the distance, Prints the result
// main code here, to run repeatedly:
{  
	digitalWrite(trigPin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trigPin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trigPin, LOW); 

	duration = pulseIn(echoPin, HIGH);
	distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);

}
//reconect if necessary
if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  if (WiFi.status() != WL_CONNECTED){
    startWifi();
  }
  // keep mqtt alive
  mqttClient.loop();

//DISTANCE DETERMINES RGB
int r, g, b;
setColorFromDistance(distance, r, g, b); // get correct RGB for current distance

//USE A PATTERN INSTEAD OF INDIVIDUAL PIXELS
for (int col = 0; col < 6; col++) {         // go column by column (6 is max LED index in one column)
  for (int row = 0; row < 12; row++) {      // go down the 12 rows (0–11)
    int pixel = col + row * 6;              // this calculates 0,6,12... then 1,7,13... etc.
    send_RGB_to_pixel(r, g, b, pixel);      // set pixel color
  }
  delay(500); // wait 0.5 seconds between each column
}

// SET RGB VALUES BASED ON DISTANCE
void setColorFromDistance(float distance, int &r, int &g, int &b) {
  if (distance < 20) { r = 53; g = 3; b = 252; } //blue
} else if (distance < 40) { r = 3; g = 12; b = 252; } // greenyblue
} else if (distance < 60) { r = 0; g = 184; b = 194; } // green
} else if (distance < 80) { r = 0; g = 173; b = 92;} // greenyyellow
} else if (distance < 100) { r = 177; g = 212; b = 2;} // greenyyellow pale
} else if (distance < 120) { r = 252; g = 236; b = 3;} // yellow
} else if (distance < 140) { r = 252; g = 169; b = 3;} // orangy yellow
} else if (distance < 160) { r = 252; g = 30; b = 3; } // orange
} else if (distance < 180) { r = 252; g = 3; b = 102; } // red
} else if (distance < 200) { r = 252; g = 3; b = 244; } // pink

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}