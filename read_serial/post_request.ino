#include <WiFi.h>
#include <HTTPClient.h>

const char WIFI_SSID[] = "YOUR_WIFI_SSID";         // CHANGE IT
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD"; // CHANGE IT

String HOST_NAME   = "http://industrial.api.ubidots.com"; 
String PATH_NAME   = "/api/v1.6/devices/My_PC";
String TOKEN       = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI";

void connectWiFi();
void disconnectWiFi(HTTPClient& http); // Pass HTTPClient object by reference
String createPayload(String context);
void sendPOSTRequest(String payload, HTTPClient& http); // Pass HTTPClient object by reference

void setup() {
  Serial.begin(9600);
  connectWiFi();
  HTTPClient http;
  String context = "Inventory is low on Compartment 1.";
  String payload = createPayload(context);
  sendPOSTRequest(payload, http);
  disconnectWiFi(http);
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void disconnectWiFi(HTTPClient& http) {
  http.end();
}

String createPayload(String context) {
  String payload = "{\"event\":{\"value\":0,\"context\":{\"notification\":\"" + context + "\"}}}";
  return payload;
}

void sendPOSTRequest(String payload, HTTPClient& http) {
  http.begin(HOST_NAME + PATH_NAME);
  http.addHeader("X-Auth-Token", TOKEN);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println(response);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

}

void loop() {
}