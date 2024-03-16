//#define PIN_RED    D11 // The Arduino Nano ESP32 pin connected to R pin
//#define PIN_GREEN  D10 // The Arduino Nano ESP32 pin connected to G pin
//#define PIN_BLUE   D9  // The Arduino Nano ESP32 pin connected to B pin
#include <HTTPClient.h>
#include <WiFi.h>
#include "time.h"
#include "pitches.h"

#define BUZZER_PIN D4

const int MAX_VARIABLES = 9; // Maximum number of VariableInfo objects
const int MAX_MEDS = 3; // Maximum number of Meds

const char WIFI_SSID[]     = "Vodafone-F81A50-Plus";
const char WIFI_PASSWORD[] = "RF4BCz3Dzn";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0; //DAYLIGHT SAVINGS ARE A MESS, SOMEHOW THE FUCTION CONSIDERS THAT WE ARE ALREADY IN DAYLIGHT SAVINGS

String HOST_NAME   = "http://industrial.api.ubidots.com"; 
String PATH_NAME   = "/api/v1.6/devices/My_PC";
String TOKEN       = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI";

void setup() {
  Serial.begin(115200); // Make sure to set the baud rate to match your Python script
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //pinMode(PIN_RED,   OUTPUT);
  //pinMode(PIN_GREEN, OUTPUT);
  //pinMode(PIN_BLUE,  OUTPUT);
}

struct VariableInfo {
  String label;
  String timestamp;
  int value;
  int lastValue;
};

struct HourMinute {
    int hour;
    int minute;
};

struct Inventory {
  int value;
  int lastValue;
};

VariableInfo variableData[MAX_VARIABLES]; // Array of VariableInfo objects
HourMinute prescribedTime[MAX_MEDS];
Inventory inventory[MAX_MEDS];
int alarmFlag[MAX_MEDS] = {0};

void loop() {
  //analogWrite(PIN_RED,   0);
  //analogWrite(PIN_GREEN, 201);
  //analogWrite(PIN_BLUE,  204);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  if (Serial.available() > 0) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
      while (Serial.available() < 0) {
      }
      String information = Serial.readStringUntil('\n');

      // Find the positions of the delimiters
      int comma1 = information.indexOf(',');
      int comma2 = information.indexOf(',', comma1 + 1);
      int comma3 = information.indexOf(',', comma2 + 1);

      // Extract substrings
      String label = information.substring(0, comma1);
      String timestamp = information.substring(comma1 + 1, comma2);
      String value = information.substring(comma2 + 1);
      String lastValue = information.substring(comma3 + 1);

      // Convert value to integer
      int intValue = value.toInt();
      int intLastValue = lastValue.toInt();

      // Create a new VariableInfo object
      VariableInfo data;
      data.label = label;
      data.timestamp = timestamp;
      data.value = intValue;
      data.lastValue = intLastValue;

      // Store the VariableInfo object in the array
      variableData[i] = data;

      // Print received data back to the serial port
      Serial.println("Received Index: " + String(i));
      Serial.println("Received Label: " + variableData[i].label);
      Serial.println("Received Timestamp: " + variableData[i].timestamp);
      Serial.println("Received Value: " + String(variableData[i].value));
      Serial.println("Received Last Value: " + String(variableData[i].lastValue));
      Serial.println(&timeinfo, "Received Time: %H:%M:%S");

      if (i < MAX_MEDS) {
        inventory[i].value = variableData[i].value;
        inventory[i].lastValue = variableData[i].lastValue;
      } else {
        if ((i - MAX_MEDS) % 2 == 0) {
          prescribedTime[(i-MAX_MEDS)/2].hour =  variableData[i].value;
        } else {
          prescribedTime[(i-MAX_MEDS-1)/2].minute =  variableData[i].value;
        }
      }
    }
  }

  for (int i = 0; i < MAX_MEDS; i++) {
    checkInventory(i);
    checkAlarm(i);
  }
}

void buzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Make the buzzer buzz at a frequency of 262 Hz
  tone(BUZZER_PIN, 262);
}

void checkInventory(int i) {
  if (inventory[i].value == 1) {
    sendHttp("URGENT: Please restock pill " + String(i+1) + ", it has only " + String(inventory[i].value) + " pill.");
  }
}

void checkAlarm(int i) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  if (alarmFlag[i] == 0) {
    if (prescribedTime[i].hour == timeinfo.tm_hour && prescribedTime[i].minute == timeinfo.tm_min) {
      if(inventory[i].value == inventory[i].lastValue) {
        Serial.println("Time to take medicine " + String(i+1));
        alarmFlag[i] = 1;
        buzzer();
      }
    } else if (inventory[i].value < inventory[i].lastValue) {
        sendHttp("The pill " + String(i+1) + " was taken at the incorrect time.");
      }
  } else if (alarmFlag[i] == 1) {
    if (inventory[i].value < inventory[i].lastValue) {
      sendHttp("The pill " + String(i+1) + " was taken at the correct time.");
      alarmFlag[i] = 0;
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
    } else if (turnOffFiveMinutes(prescribedTime[i]) == 1) {
      sendHttp("Already passed 5 minutes since the time to take the pill " + String(i+1) + ".");
      alarmFlag[i] = 0;
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
    }
  }
}

//check if 5 Minutes have already passed
int turnOffFiveMinutes(struct HourMinute prescribedTime) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return -1;
  }
  if ((prescribedTime.minute >= 55 && timeinfo.tm_min >= (prescribedTime.minute + 5 - 60)) || (timeinfo.tm_min >= (prescribedTime.minute + 5))) {
    if ((prescribedTime.hour == 23 && timeinfo.tm_hour == 0) || (prescribedTime.hour == timeinfo.tm_hour)) {
      return 1;
    }
  }
  return 0;
}

//check if no pills are in the 5 min interval to take pills (buzzer purposes)
int checkPositiveFlag() {
  for(int j = 0; j < MAX_MEDS; j++) {
    if (alarmFlag[j] == 1) {
      return 1;
    }
  }
  return 0;
}

void stopBuzzer() {
  noTone(BUZZER_PIN);
}

void sendHttp(String context) {
  HTTPClient http;
  connectWiFi();
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

String createPayload(String context) {
  String payload = "{\"event\":{\"value\":0,\"context\":{\"notification\":\"" + context + "\"}}}";
  return payload;
}

void disconnectWiFi(HTTPClient& http) {
  http.end();
}
