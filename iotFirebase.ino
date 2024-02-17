#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <DHT.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Firebase credentials
#define API_KEY ""
#define DATABASE_URL ""

// Pin Definitions
#define BUZZER_PIN 13
#define LDR_PIN_TOP 32
#define LDR_PIN_DOWN 33
#define LDR_PIN_LEFT 34
#define LDR_PIN_RIGHT 35
#define SERVO_PIN_HORIZONTAL 18
#define SERVO_PIN_VERTICAL 19
#define DHT_PIN 21
#define DHT_TYPE DHT11

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Servo objects
Servo servoMotor1; // Horizontal servo
Servo servoMotor2; // Vertical servo

// DHT sensor object
DHT dht(DHT_PIN, DHT_TYPE);

// Variables
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int ldrDataTop = 0;
int ldrDataDown = 0;
int ldrDataLeft = 0;
int ldrDataRight = 0;
bool buzzerStatus = false;
int horizontalStatus = 0;
int verticalStatus = 0;
int mins = 0;
float humidity = 0;
float temperature = 0;

// Function to set up Wi-Fi connection
void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

// Function to set up Firebase connection
void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase signup failed: %s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// Function to set up servo motors
void setupServo() {
  servoMotor1.attach(SERVO_PIN_HORIZONTAL);
  servoMotor2.attach(SERVO_PIN_VERTICAL);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();

  setupWiFi();      // Set up Wi-Fi connection
  setupFirebase(); // Set up Firebase connection
  setupServo();    // Set up servo motors
}

// Function to send sensor data to Firebase
void sendDataToFirebase() {
  // Read and send LDR sensor data
  ldrDataTop = map(analogRead(LDR_PIN_TOP), 0, 4095, 0, 100);
  Firebase.set("/ESP/sensor/ldrDataTop", ldrDataTop);
  
  ldrDataDown = map(analogRead(LDR_PIN_DOWN), 0, 4095, 0, 100);
  Firebase.set("/ESP/sensor/ldrDataDown", ldrDataDown);
  
  ldrDataLeft = map(analogRead(LDR_PIN_LEFT), 0, 4095, 0, 100);
  Firebase.set("/ESP/sensor/ldrDataLeft", ldrDataLeft);
  
  ldrDataRight = map(analogRead(LDR_PIN_RIGHT), 0, 4095, 0, 100);
  Firebase.set("/ESP/sensor/ldrDataRight", ldrDataRight);
  
  // Send minutes data
  if ((millis() - sendDataPrevMillis) >= 60000) {
    sendDataPrevMillis = millis();
    mins++;
    Firebase.set("/ESP/minutes", mins);
  }
  
  // Read and send DHT sensor data
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (!isnan(humidity) && !isnan(temperature)) {
    Firebase.set("/ESP/sensor/Humidity", humidity);
    Firebase.set("/ESP/sensor/temperature", temperature);
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }
}

// Function to receive data from Firebase
void receiveDataFromFirebase() {
  // Check if there is data available for buzzer
  if (Firebase.RTDB.getBool(&fbdo, "ESP/buzzer")) {
    if (fbdo.dataType() == "boolean") {
      buzzerStatus = fbdo.boolData();
      digitalWrite(BUZZER_PIN, buzzerStatus ? HIGH : LOW);
    }
  }

  // Check if there is data available for horizontal servo
  if (Firebase.RTDB.getInt(&fbdo, "ESP/servo/horizontal")) {
    if (fbdo.dataType() == "int") {
      horizontalStatus = fbdo.intData();
      servoMotor1.write(horizontalStatus);
      delay(15);
    }
  }

  // Check if there is data available for vertical servo
  if (Firebase.RTDB.getInt(&fbdo, "ESP/servo/vertical")) {
    if (fbdo.dataType() == "int") {
      verticalStatus = fbdo.intData();
      servoMotor2.write(verticalStatus);
      delay(15);
    }
  }
}

void loop() {
  // Check if Firebase is ready and signup is successful, and if it's time to send data
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){    
    sendDataToFirebase();      // Send data to Firebase
    receiveDataFromFirebase(); // Receive data from Firebase
  }
}
