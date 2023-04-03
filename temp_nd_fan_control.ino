// Include the libraries for temperature sensor and WiFi
#include <DHT.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Define the pin for temperature sensor
#define DHTPIN 2

// Select the type of temperature sensor (DHT11 or DHT22)
#define DHTTYPE DHT11 

// Define the pin for the fan
#define FAN_PIN 3

// Create an instance of the temperature sensor
DHT dht(DHTPIN, DHTTYPE);

// Create an instance of the WiFi server
WiFiServer server(80);

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Initialize the temperature sensor
  dht.begin();

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Start the server
  server.begin();

  // Set the fan pin as output
  pinMode(FAN_PIN, OUTPUT);
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait for the client to send a request
  while (!client.available()) {
    delay(1);
  }

  // Read the request from the client
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Check if the request is for turning on the fan
  if (request.indexOf("/fan/on") != -1) {
    digitalWrite(FAN_PIN, HIGH);
  }

  // Check if the request is for turning off the fan
  if (request.indexOf("/fan/off") != -1) {
    digitalWrite(FAN_PIN, LOW);
  }

  // Read the temperature from the sensor
  float temperature = dht.readTemperature();

  // Check if the temperature is valid
  if (!isnan(temperature)) {
    // Send the temperature to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.print("Temperature: ");
    client.print(temperature);
    client.print(" °C");
  } else {
    // Send an error message to the client
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/html");
    client.println();
    client.print("Failed to read temperature");
  }

  // Close the connection
  delay(10);
  client.stop();
}
