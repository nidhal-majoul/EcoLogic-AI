#include <WiFi.h>
#include <WebServer.h>

// NETWORK CREDENTIALS 
// These must match your WIFI settings
const char* ssid = "YOUR WIFI";     
const char* password = "YOUR PASSWORD";   

// HARDWARE CONFIGURATION
// Pin 48 is the built-in RGB LED on most ESP32-S3 DevKits.
#define MY_LED 48 

// Initialize the web server on port 80 (standard HTTP port)
WebServer server(80);

/**
 * Handle "ECO" requests from Python.*/
void handleEco() {
  digitalWrite(MY_LED, HIGH); // Turn LED ON (Physical signal for Eco Mode)
  server.send(200, "text/plain", "ECO MODE ACTIVE");
  Serial.println(">>> HTTP Command Received: Switching to ECO MODE");
}

/**
 * Handle "NORMAL" requests from Python.
 */
void handleNormal() {
  digitalWrite(MY_LED, LOW); // Turn LED OFF (Physical signal for Normal Mode)
  server.send(200, "text/plain", "NORMAL MODE ACTIVE");
  Serial.println(">>> HTTP Command Received: Switching to NORMAL MODE");
}

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  
  // Set the LED pin as an output
  pinMode(MY_LED, OUTPUT);

  //WIFI CONNECTION 
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Display the IP address
  Serial.println("\n[SUCCESS] WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //SERVER ROUTING 
  // Link the URL paths to the functions defined above
  server.on("/ECO", handleEco);
  server.on("/NORMAL", handleNormal);

  // Start the server
  server.begin();
  Serial.println("HTTP Server Started. Ready for AI commands.");
}

void loop() {
  // Continuously check for incoming HTTP requests from the Python Vision script
  server.handleClient();
}
