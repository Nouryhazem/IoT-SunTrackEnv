
// Include necessary libraries
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h> 

// WiFi credentials
char ssid[] = ""; // Insert your WiFi SSID
char pass[] = ""; // Insert your WiFi password

// Blynk authentication token and template details
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// DHT sensor pin and type
#define DHTPIN 21   
#define DHTTYPE DHT11  

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Define servo pins
const int horizontalPin = 19; // Connect to horizontal servo
const int verticalPin = 18;   // Connect to vertical servo

// Define servo objects
Servo horizontalServo;
Servo verticalServo;

int Tolerence = 90;//initializing variable for error

//Define  the 4 LDR sensors
const int sensorPinUp = 32;// up 
const int sensorPinDown = 33;//DOWN 
const int sensorPinRight = 34;//right 
const int sensorPinLeft = 35;//left 

// Read DHT sensor data
float humidity = 0;
float temperature = 0;

// Read LDR sensors values
int sensorUp = 0; // read the current light levels
int sensorDown = 0; // read the current light levels
int sensorRight = 0; // read the current light levels
int sensorLeft = 0; // read the current light levels

// Mode variables
int mode = 0;
int appPos1 = 0;
int appPos2 = 0;


// Function to receive mode selection from the app (AUTO or MANUAL)
BLYNK_WRITE(V9) // Slider Widget for selecting mode (AUTO or MANUAL)
{
  mode = param.asInt(); // Get the selected mode (0 for MANUAL, 1 for AUTO)
  Serial.print("Mode: ");
  Serial.println(mode);// Print selected mode
}

// Function to receive manual horizontal servo position from the app
BLYNK_WRITE(V6) // Slider Widget for manual horizontal servo control
{
  appPos1 = param.asInt(); // Get the manual horizontal servo position
  Serial.print(" Manual horizontal servo position: ");
  Serial.println(appPos1);
}

// Function to receive manual vertical servo position from the app
BLYNK_WRITE(V8) // Slider Widget for manual vertical servo control
{
  appPos2 = param.asInt(); // Get the manual vertical servo position
  Serial.print("Manual vertical servo position: ");
  Serial.println(appPos2);
}



void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  connectToWiFi();

  // Begin Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize DHT sensor
  dht.begin();

  pinMode(13, OUTPUT); //Buzzer pin

  // Attach servo objects to pins
  horizontalServo.attach(horizontalPin);
  verticalServo.attach(verticalPin);  

  // Set initial servo positions
  horizontalServo.write(100); // sets the angle of servo 1 to 90 degrees
  verticalServo.write(100); // sets the angle of servo 2 to 90 degrees

  delay(2000); // giving a delay of 2 seconds

}


void loop() {
  Blynk.run();
  sys();

  
}

// Function to connect to WiFi
void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void sys() {
  // Read DHT sensor data
   humidity = dht.readHumidity();
   temperature = dht.readTemperature();

  // Read LDR sensors values
   sensorUp = analogRead(sensorPinUp); // read the current light levels
   sensorDown = analogRead(sensorPinDown); // read the current light levels
   sensorRight = analogRead(sensorPinRight); // read the current light levels
   sensorLeft = analogRead(sensorPinLeft); // read the current light levels


  // Check if DHT sensor readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print sensor readings
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print(" %\tTemperature: ");
  Serial.println(temperature);
  Serial.print("sensorup");
  Serial.println(sensorUp);
  Serial.print("sensorleft");
  Serial.println(sensorLeft);
  Serial.print("sensordown");
  Serial.println(sensorDown);
  Serial.print("sensorright");
  Serial.println(sensorRight);

  // Send sensor readings to Blynk app
  Blynk.virtualWrite(V1, temperature);   // Send humidity data to V0 in the Blynk app
  Blynk.virtualWrite(V2, humidity); // Send temperature data to V1 in the Blynk app
  Blynk.virtualWrite(V3, sensorUp);
  Blynk.virtualWrite(V4, sensorDown);
  Blynk.virtualWrite(V5, sensorRight);
  Blynk.virtualWrite(V7, sensorLeft);


  // Check temperature threshold and activate buzzer
  if (temperature > 60) {
    activateBuzzer(temperature);
  }

// Automatic mode
  if (mode == 1) {
    // Adjust servo positions based on sensor readings
    adjustServoPositions(sensorUp, sensorDown, sensorRight, sensorLeft);
    delay(60);
    } else{
    // Manual mode - control servos based on manual input from the app
    controlServosManually();
    delay(100);
    }

}



// Function to activate buzzer based on temperature
void activateBuzzer(float temperature) {
  Serial.println("Buzzer is on");
  int toneDuration = map(temperature, 30, 40, 2000, 500);
  digitalWrite(13, HIGH);
  tone(13, 1000, toneDuration);
  digitalWrite(13, LOW);
  delay(1000);
}


// Function to adjust servo positions based on sensor readings
void adjustServoPositions(int sensorUp, int sensorDown, int sensorRight, int sensorLeft) {
  // Calculate sensor differences
  int sensorDiffUp = abs(sensorUp - sensorDown);
  int sensorDiffRight = abs(sensorRight - sensorLeft);

  // Control vertical servo based on sensor readings
  if (sensorDiffUp >= Tolerence) {
    adjustVerticalServo(sensorUp, sensorDown);
  }

  // Control horizontal servo based on sensor readings
  if (sensorDiffRight >= Tolerence) {
    adjustHorizontalServo(sensorLeft, sensorRight);
  }
}

// Function to adjust vertical servo
void adjustVerticalServo(int sensorUp, int sensorDown) {
  int currentPos = verticalServo.read();
  if (sensorUp < sensorDown) {
    currentPos -= 5;
  } else {
    currentPos += 6;
  }
  currentPos = constrain(currentPos, 90, 170);
  verticalServo.write(currentPos);
}

// Function to adjust horizontal servo
void adjustHorizontalServo(int sensorLeft, int sensorRight) {
  int currentPos = horizontalServo.read();
  if (sensorLeft > sensorRight) {
    currentPos += 6;
  } else {
    currentPos -= 5;
  }
  currentPos = constrain(currentPos, 20, 170);
  horizontalServo.write(currentPos);
}

// Function to control servos manually
void controlServosManually() {
  appPos1 = constrain(appPos1, 0, 160);
  horizontalServo.write(appPos1);
  appPos2 = constrain(appPos2, 40, 180);
  verticalServo.write(appPos2);
}