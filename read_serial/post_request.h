#ifndef UBIDOTS_HTTP_H
#define UBIDOTS_HTTP_H

#include <WiFi.h>
#include <HTTPClient.h>

const char WIFI_SSID[] = "YOUR_WIFI_SSID";         // CHANGE IT
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD"; // CHANGE IT

String HOST_NAME   = "http://industrial.api.ubidots.com"; 
String PATH_NAME   = "/api/v1.6/devices/My_PC";
String TOKEN       = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI";

void connectWiFi();
void disconnectWiFi(HTTPClient& http);
String createPayload(String context);
void sendPOSTRequest(String payload, HTTPClient http&);

#endif
