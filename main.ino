#include <SoftwareSerial.h>
#include "DHT.h"

// Pin configuration
const int RX_PIN = 10;              // RX pin for ESP8266 communication
const int TX_PIN = 11;              // TX pin for ESP8266 communication
const int DHT_PIN = 6;              // Pin connected to the DHT sensor data line

// Initialize variables
float minVpd = 0.4; // Minimum VPD threshold
float maxVpd = 0.8; // Maximum VPD threshold
float leafDifference = -2.8; // Difference between leaf temperature and ambient temperature
bool isDehumidifierOn = false; // State of the dehumidifier (on/off)
bool isHumidifierOn = false; // State of the humidifier (on/off)

SoftwareSerial ESP_Serial(RX_PIN, TX_PIN);
DHT dht(DHT_PIN, DHT11);

void setup() {
  Serial.begin(9600);   // Initialize serial communication with PC at 9600 baud
  ESP_Serial.begin(9600);   // Initialize serial communication with ESP8266 at 9600 baud
  dht.begin();   // Initialize DHT sensor

  delay(4000);
  ESP_Serial.println("AT+RST");   // Reset ESP8266 module
  delay(2000);
  ESP_Serial.println("AT+CWMODE=1");   // Set ESP8266 mode to client mode
}

void loop() {
  delay(886000);   // Delay of 14 minutes and 46 seconds

   // Read temperature and humidity from DHT sensor
  float temp = readTemperature();
  float hum = readHumidity();

  // Calculate the VPD (Vapor Pressure Deficit)
  float vpd = (610.8 * pow(10, ((7.5 * (temp + leafDifference)) / (237.3 + temp + leafDifference))) / (1000)) - (610.8 * pow(10, ((7.5 * temp) / (237.3 + temp))) / (1000)) * hum / 100;
  
  // Control the humidity based on VPD
  controlHumidity(vpd, minVpd, maxVpd);

  // Send sensor data
  sendData(temperature, humidity);
}


// Function to control the humidity based on VPD
void controlHumidity(float vpd, float minVpd, float maxVpd) {
    // Determine the desired states of the dehumidifier and humidifier based on humidity conditions
    bool dehumidifierOn = (vpd < minVpd);
    bool humidifierOn = (vpd > maxVpd);

    // Control the dehumidifier and humidifier
    controlDehumidifier(dehumidifierOn);
    controlHumidifier(humidifierOn);
}

// Function to send data using GET request
void sendData(float temperature, int humidity) {
  delay(1000);
  ESP_Serial.println("AT+CWJAP=\"YOUR_SSID\",\"YOUR_PASSWORD\"");   // ********UR WIFI CREDENTIALS HERE*********
  delay(10000);

  ESP_Serial.println("AT+CIPSTART=\"TCP\",\"api.pushingbox.com\",80");   // Connect to server
  delay(1000);

  // Construct the URL with temperature and humidity values
  String url = "/pushingbox?devid=YOUR_ID"; // ********UR ID HERE*********
  url += "&LDR=" + String(temperature);
  url += "&Button=" + String(humidity);

  // Prepare the HTTP GET request
  String request = "GET " + url + " HTTP/1.1\r\nHost: api.pushingbox.com\r\n\r\n";
  int contentLength = request.length();

  ESP_Serial.println("AT+CIPSEND=" + String(contentLength));   // Send request length
  delay(1000);

  ESP_Serial.print(request);   // Send HTTP GET request
  delay(1000);

  ESP_Serial.println("AT+CIPCLOSE");   // Close the connection
}

// Function to control the dehumidifier
void controlDehumidifier(bool turnOn) {
    // Check if the dehumidifier is already in the desired state
    if (isDehumidifierOn == turnOn)
        return; // No change needed, return

    // Update the state of the dehumidifier
    isDehumidifierOn = turnOn;

    // Code to turn on or off the dehumidifier goes here
    // NOT IMPLEMENTED
}

// Function to control the humidifier
void controlHumidifier(bool turnOn) {
    // Check if the humidifier is already in the desired state
    if (isHumidifierOn == turnOn)
        return; // No change needed, return

    // Update the state of the humidifier
    isHumidifierOn = turnOn;

    // Code to turn on or off the humidifier goes here
    // NOT IMPLEMENTED
}

// Function to read temperature from DHT sensor
float readTemperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    temperature = 0.00;
  }
  return temperature;
}

// Function to read humidity from DHT sensor
float readHumidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    humidity = 0.00;
  }
  return humidity;
}