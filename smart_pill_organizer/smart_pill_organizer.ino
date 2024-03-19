#include <HTTPClient.h>
#include <WiFi.h>
#include "time.h"

#define BUZZER_PIN D12
#define PIN_RED D2
#define PIN_GREEN_1 D3 
#define PIN_GREEN_2 D4     
#define PIN_GREEN_3 D5
                                                                                                

const int MAX_VARIABLES = 9; // Maximum number of VariableInfo objects
const int MAX_PILLS = 3; // Maximum number of pills
const int medPresPins[3] = {PIN_GREEN_1, PIN_GREEN_2, PIN_GREEN_3};

const char WIFI_SSID[]     = "Vodafone-F81A50-Plus"; //change the ssid according to your wifi
const char WIFI_PASSWORD[] = "RF4BCz3Dzn"; //your wifi password

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0; //The function already considers GMT+0 in daylight savings

const unsigned long interval = 600000; // (10 minutes) Interval in milliseconds of the time to send an email in case of low pill count
unsigned long previousMillis = 0; 

String HOST_NAME   = "http://industrial.api.ubidots.com"; 
String PATH_NAME   = "/api/v1.6/devices/smart_pill_organizer";
String TOKEN       = "BBUS-zjuPmVgxisCTzVOcQ1wUjwNDxaPcsI";

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
HourMinute prescribedTime[MAX_PILLS];
Inventory inventory[MAX_PILLS];

int alarmFlag[MAX_PILLS] = {0}; //Normal status = 0, Alarm ring = 1
int statusFlag = 0; //Status normal

String startDownTime;
String endDownTime;
String message;

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
}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  message = "";

  if (Serial.available() > 0) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
      while (Serial.available() <= 0) {
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

      if (label.startsWith("Error") || label.startsWith("Exception")) {
        if (statusFlag == 0) {
          struct tm timeinfo;
          if(!getLocalTime(&timeinfo)){
              Serial.println("Failed to obtain time");
          return;
          }
          startDownTime = createDateString(timeinfo);
          analogWrite(PIN_RED, 255);
          statusFlag = 1;
        }
        return;
      } else {
        if (statusFlag == 1) {
          struct tm timeinfo;
          if(!getLocalTime(&timeinfo)){
              Serial.println("Failed to obtain time");
          return;
          }
          endDownTime = createDateString(timeinfo);

          sendHttp("UPDATE: Cloud is back. It was down from " + startDownTime + " to " + endDownTime + ". You may receive some unexpected alerts.");
          statusFlag = 0;
          analogWrite(PIN_RED, 0);
        }
      }

      // Store the VariableInfo object in the array
      variableData[i] = data;

      // Print received data back to the serial port
      Serial.println("Received Index: " + String(i));
      Serial.println("Received Label: " + variableData[i].label);
      Serial.println("Received Timestamp: " + variableData[i].timestamp);
      Serial.println("Received Value: " + String(variableData[i].value));
      Serial.println("Received Last Value: " + String(variableData[i].lastValue));
      Serial.println(&timeinfo, "Received Time: %H:%M:%S");

      if (i < MAX_PILLS) {
        inventory[i].value = variableData[i].value;
        inventory[i].lastValue = variableData[i].lastValue;
      } else {
        if ((i - MAX_PILLS) % 2 == 0) {
          prescribedTime[(i-MAX_PILLS)/2].hour =  variableData[i].value;
        } else {
          prescribedTime[(i-MAX_PILLS-1)/2].minute =  variableData[i].value;
          checkAlarm((i-MAX_PILLS-1)/2);
        }
      }
    }
  }

  unsigned long currentMillis = millis(); // Get the current time

  // Check if the specified interval has passed
  if (currentMillis - previousMillis >= interval) {
    // Update the last time the loop ran
    previousMillis = currentMillis;
    for (int i = 0; i < MAX_PILLS; i++) {
      checkInventory(i);
    }
  }
  if (!message.equals("")) {
    sendHttp(message);
  }
}

String sendMessage(String msg) {
  message += (msg + " ");
  return message;
} 

String createDateString(struct tm timeinfo) {
  char weekDay[10];
  strftime(weekDay, 10, "%A", &timeinfo);
  char month[10];
  strftime(month, 10, "%B", &timeinfo);
  char year[5];
  strftime(year, 5, "%Y", &timeinfo);
  char day[3];
  strftime(day, 3, "%d", &timeinfo);
  char hour[3];
  strftime(hour, 3, "%H", &timeinfo);
  char min[3];
  strftime(min, 3, "%M", &timeinfo);
  char sec[3];
  strftime(sec, 3, "%S", &timeinfo);

  String date = String(weekDay) + ", " + String(month) + " " + String(day) + " " + String(year) + " " + String(hour) + ":" + String(min) + ":" + String(sec);
  return date;
}

void buzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  // Make the buzzer buzz at a frequency of 262 Hz
  tone(BUZZER_PIN, 262);
}

void checkInventory(int i) {
  if (inventory[i].value <= 1) {
    sendMessage("Please restock pill " + String(i+1) + ", it only has " + String(inventory[i].value) + ".");
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
        message = sendMessage("Time to take pill " + String(i+1) + ".");
        alarmFlag[i] = 1;
        buzzer();
        analogWrite(medPresPins[i], 255);  
      }
    } else if (inventory[i].value < inventory[i].lastValue) {
        message = sendMessage("The pill " + String(i+1) + " was taken at the incorrect time.");
      }
  } else if (alarmFlag[i] == 1) {
    if (inventory[i].value < inventory[i].lastValue) {
      message = sendMessage("The pill " + String(i+1) + " was taken at the correct time.");
      alarmFlag[i] = 0;
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
      analogWrite(medPresPins[i], 0);
    } else if (turnOffFiveMinutes(prescribedTime[i]) == 1) {
      message = sendMessage("5 minutes have passed since the time to take the pill " + String(i+1) + ".");
      alarmFlag[i] = 0;
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
      analogWrite(medPresPins[i], 0);
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
  if (prescribedTime.minute >= 55 && timeinfo.tm_min >= (prescribedTime.minute + 5 - 60)) {
    if ((prescribedTime.hour == 23 && timeinfo.tm_hour == 0) || (prescribedTime.hour + 1 == timeinfo.tm_hour)) {
      return 1;
    }
  } else if ((timeinfo.tm_min >= (prescribedTime.minute + 5)) && (prescribedTime.hour == timeinfo.tm_hour)) {
    return 1;
  }
  return 0;
}


//check if no pills are in the 5 min interval to take pills (buzzer purposes)
int checkPositiveFlag() {
  for(int j = 0; j < MAX_PILLS; j++) {
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
