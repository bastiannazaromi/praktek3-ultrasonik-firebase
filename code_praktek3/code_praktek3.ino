// Wifi
#include <ESP8266WiFi.h>
#include "WiFiClient.h"
#include "WiFiServer.h"
#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include "FirebaseESP8266.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>

FirebaseData firebaseData; //Membuat Variabel firebaseData
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

#define WIFI_SSID "xxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxx"

#define FIREBASE_HOST "xxxxxxxxxx"
#define FIREBASE_AUTH "xxxxxxxxxx"

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

// Sensor Ultrasonik
#define echoPin D7
#define trigPin D6

String statusKipas = "";
#define pinRelay D2

#define relay_off HIGH
#define relay_on LOW

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.println();
  Serial.print("connecting to wi-fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  
  Serial.println("Sukses terkoneksi wifi!");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  //Firebase Setup
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, relay_off);
}

void loop() {
  int durasi, jarak, pos=0;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  durasi = pulseIn(echoPin, HIGH);
  jarak = (durasi / 2) / 29.1;

  if (jarak < 0) {
    jarak = 0;
  } else if (jarak > 100) {
    jarak = 100;
  }

  Serial.print("Jarak : ");
  Serial.print(jarak);
  Serial.println(" cm");

  Serial.println();

  if (Firebase.getString(firebaseData, "/status/kipas")) {
    if  (firebaseData.dataType() == "string")
    {
      statusKipas = firebaseData.stringData();
    }
  }

  if (statusKipas == "ON") {
    Serial.println("Status Relay : ON");
  } else {
    Serial.println("Status Relay : OFF");
  }
    
  if ((millis() - lastTime) > timerDelay) {
    //Kirim Data Sensor Waterflow ke Firebase
    Firebase.setString(firebaseData, "data/jarak", jarak);

    lastTime = millis();
  }

  Serial.println();
  delay(1000);
}
