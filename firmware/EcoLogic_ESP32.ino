#include <WiFi.h>

const char* ssid = "YOUR WIFI"; 
const char* password = "YOUR_PASSWORD"; 

WiFiServer server(80);
const int LED_PIN = 2; 

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Serial.println("\nConnected!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    if (req.indexOf("/ECO") != -1) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Hardware: ECO ON");
    } else if (req.indexOf("/NORMAL") != -1) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Hardware: NORMAL MODE");
    }
    client.stop();
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    
    if (request.indexOf("GET /ECO") >= 0) {
      digitalWrite(INTERNAL_LED, HIGH); // Light ON
      Serial.println("Mode: ECO - LED ON");
    } 
    else if (request.indexOf("GET /NORMAL") >= 0) {
      digitalWrite(INTERNAL_LED, LOW);  // Light OFF
      Serial.println("Mode: NORMAL - LED OFF");
    }
    client.stop();
  }
}
