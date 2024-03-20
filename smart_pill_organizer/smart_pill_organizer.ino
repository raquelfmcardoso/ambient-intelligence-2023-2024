#include <HTTPClient.h>
#include <WiFi.h>
#include "time.h"

#define PIN_BUZZER D12
#define PIN_RED D2
#define PIN_GREEN_1 D3 
#define PIN_GREEN_2 D4     
#define PIN_GREEN_3 D5                                                                             

String HOST_NAME   = "http://industrial.api.ubidots.com"; 
String PATH_NAME   = "/api/v1.6/devices/smart_pill_organizer";
String TOKEN       = "BBUS-6r8wUc13PXf3brHoIIKQbZzlk5BEP5";

const char WIFI_SSID[]     = "Vodafone-F81A50-Plus"; // Replace the SSID with your own
const char WIFI_PASSWORD[] = "RF4BCz3Dzn"; // Replace the password with your own

const char* ntpServer = "pool.ntp.org"; // Define the NTP server address
const long gmtOffset_sec = 0; // Define the GMT offset in seconds. This is the time difference between your local time and GMT time
const int daylightOffset_sec = 0; // Define the daylight saving time (DST) offset in seconds

const int MAX_VARIABLES = 9; // Maximum number of VariableInfo objects
const int MAX_PILLS = 3; // Maximum number of pills
const int PILL_PINS[3] = {PIN_GREEN_1, PIN_GREEN_2, PIN_GREEN_3}; // The pins used to represent the pills
const int FREQUENCY = 262; // Buzzer frequency (262Hz)

const unsigned long interval = 600000; // Interval in milliseconds of the time to send an email in case of low pill count (10 minutes)
unsigned long previousMillis = 0;

int pillEventFlag[MAX_PILLS] = {0}; // Normal status = 0, event triggered = 1
int statusFlag = 0; // Normal status = 0, Ubidots down = 1

String message; // Variable to store the message to be sent in the email

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

struct DownTime {
  String start;
  String end;
};

VariableInfo variableData[MAX_VARIABLES]; // Array of VariableInfo objects
HourMinute prescribedTime[MAX_PILLS]; //Array of HourMinute objects
Inventory inventory[MAX_PILLS]; //Array of Inventory objects
DownTime downTime; // Variable used to control the downtime of ubidots 


void setup() {
  // Make sure to set the baud rate to match your Python script
  Serial.begin(115200);
  // Connect to the WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure time settings
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Obtain local time
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // Disconnect from WiFi and turn off WiFi mode
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Set pin modes for various pins
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN_1, OUTPUT);
  pinMode(PIN_GREEN_2, OUTPUT);
  pinMode(PIN_GREEN_3, OUTPUT);
}

void loop() {
  // Obtain local time
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  message = "";

  // Check if data is available on the serial port
  if (Serial.available() > 0) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
      while (Serial.available() <= 0) {
      }
      // Read serial data until newline character
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

      // Check for ubidots server status
      if (label.startsWith("Error") || label.startsWith("Exception")) {
        if (statusFlag == 0) {
          // Record downtime
          struct tm timeinfo;
          if(!getLocalTime(&timeinfo)){
              Serial.println("Failed to obtain time");
          return;
          }
          downTime.start = createDateString(timeinfo);
           // Indicate server down status
          statusFlag = 1;
          analogWrite(PIN_RED, 255);
        }
        return;
      } else {
        if (statusFlag == 1) {
          // Record uptime
          struct tm timeinfo;
          if(!getLocalTime(&timeinfo)){
              Serial.println("Failed to obtain time");
          return;
          }
          downTime.end = createDateString(timeinfo);
          // Indica the server is up and send an email
          sendHttp("UPDATE: Cloud is back. It was down from " + downTime.start + " to " + downTime.end + ". You may receive some unexpected alerts.");
          statusFlag = 0;
          analogWrite(PIN_RED, 0);
        }
      }

      //Store the received data and print it to the serial port
      variableData[i] = data;
      Serial.println("Received Index: " + String(i));
      Serial.println("Received Label: " + variableData[i].label);
      Serial.println("Received Timestamp: " + variableData[i].timestamp);
      Serial.println("Received Value: " + String(variableData[i].value));
      Serial.println("Received Last Value: " + String(variableData[i].lastValue));
      Serial.println(&timeinfo, "Received Time: %H:%M:%S");

      // Store values received in arrays and check for pill events
      if (i < MAX_PILLS) {
        inventory[i].value = variableData[i].value;
        inventory[i].lastValue = variableData[i].lastValue;
      } else {
        if ((i - MAX_PILLS) % 2 == 0) {
          prescribedTime[(i-MAX_PILLS)/2].hour =  variableData[i].value;
        } else {
          prescribedTime[(i-MAX_PILLS-1)/2].minute =  variableData[i].value;
          checkPillsEvent((i-MAX_PILLS-1)/2);
        }
      }
    }
  }

  // Get the current time in milliseconds since the Arduino board began running
  unsigned long currentMillis = millis(); 
  // Check if the specified interval has passed to check the inventory status of the pill
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    for (int i = 0; i < MAX_PILLS; i++) {
      checkInventory(i);
    }
  }
  // Check for messages to be send
  if (!message.equals("")) {
    sendHttp(message);
  }
}

// Check if the inventory is low (less than or equal to 1 pill)
void checkInventory(int i) {
  if (inventory[i].value <= 1) {
    sendMessage("Please restock pill " + String(i+1) + ", it only has " + String(inventory[i].value) + ".");
  }
}

// check the status for a specific type of pill
void checkPillsEvent(int i) {
  // Get the current local time
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  // Check if there is already an event triggered for the pill
  if (pillEventFlag[i] == 0) {
    // Check if the current time matches the prescribed time for taking the pill
    if (prescribedTime[i].hour == timeinfo.tm_hour && prescribedTime[i].minute == timeinfo.tm_min) {
       // Check if the inventory level is consistent (pill not taken)
      if(inventory[i].value == inventory[i].lastValue) {
        message = sendMessage("Time to take pill " + String(i+1) + ".");
        pillEventFlag[i] = 1;
        buzzer();
        analogWrite(PILL_PINS[i], 255);  
      }
      // If the pill was taken at an incorrect time
    } else if (inventory[i].value < inventory[i].lastValue) {
        message = sendMessage("The pill " + String(i+1) + " was taken at the incorrect time.");
      }
    // If pill event flag is already set
  } else if (pillEventFlag[i] == 1) {
    // If the pill is taken at the correct time
    if (inventory[i].value < inventory[i].lastValue) {
      message = sendMessage("The pill " + String(i+1) + " was taken at the correct time.");
      pillEventFlag[i] = 0;
      // If all pill events are off, stop the buzzer
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
      analogWrite(PILL_PINS[i], 0);
      // If 5 minutes have passed since the prescribed time
    } else if (turnOffFiveMinutes(prescribedTime[i]) == 1) {
      message = sendMessage("5 minutes have passed since the time to take the pill " + String(i+1) + ".");
      pillEventFlag[i] = 0;
      // If all pill events are off, stop the buzzer
      if (checkPositiveFlag() == 0) {
        stopBuzzer();
      }
      analogWrite(PILL_PINS[i], 0);
    }
  }
}

// Make the buzzer buzz at a determined frequency 
void buzzer() {
  tone(PIN_BUZZER, FREQUENCY);
}

// Turns off the buzzer sound
void stopBuzzer() {
  noTone(PIN_BUZZER);
}

// Check if any pills are within the 5-minute interval for taking pills (for buzzer purposes)
int checkPositiveFlag() {
  for(int j = 0; j < MAX_PILLS; j++) {
    if (pillEventFlag[j] == 1) {
      return 1;
    }
  }
  return 0;
}

// Check if 5 minutes have already passed since the prescribed time
int turnOffFiveMinutes(struct HourMinute prescribedTime) {
  // Get the current local time
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return -1;
  }
  // Check if the prescribed minute is close to 55 and if 5 minutes later it would be in the next hour
  if (prescribedTime.minute >= 55 && timeinfo.tm_min >= (prescribedTime.minute + 5 - 60)) {
    if ((prescribedTime.hour == 23 && timeinfo.tm_hour == 0) || (prescribedTime.hour + 1 == timeinfo.tm_hour)) {
      return 1;
    }
  // Check if 5 minutes have passed within the same hour
  } else if ((timeinfo.tm_min >= (prescribedTime.minute + 5)) && (prescribedTime.hour == timeinfo.tm_hour)) {
    return 1;
  }
  return 0;
}

// Concatenate messages to be sent in the email
String sendMessage(String msg) {
  message += (msg + " ");
  return message;
} 

// Create a formatted date string from a given struct tm timeinfo
String createDateString(struct tm timeinfo) {
  char weekDay[10];
  char month[10];
  char year[5];
  char day[3];
  char hour[3];
  char min[3];
  char sec[3];

  // Extract and format individual components of the date
  strftime(weekDay, 10, "%A", &timeinfo);
  strftime(month, 10, "%B", &timeinfo);
  strftime(year, 5, "%Y", &timeinfo);
  strftime(day, 3, "%d", &timeinfo);
  strftime(hour, 3, "%H", &timeinfo);
  strftime(min, 3, "%M", &timeinfo);
  strftime(sec, 3, "%S", &timeinfo);

  String date = String(weekDay) + ", " + String(month) + " " + String(day) + " " + String(year) + " " + String(hour) + ":" + String(min) + ":" + String(sec);
  return date;
}

// Send an HTTP POST request with the provided context
void sendHttp(String context) {
  HTTPClient http;
  connectWiFi();
  String payload = createPayload(context);
  sendPOSTRequest(payload, http);
  disconnectWiFi(http);
}

// Connect to a WiFi network using the provided SSID and password
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

// Send an HTTP POST request with the provided payload using the specified HTTP client
void sendPOSTRequest(String payload, HTTPClient& http) {
  http.begin(HOST_NAME + PATH_NAME);
  http.addHeader("X-Auth-Token", TOKEN);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);

  // Check if the request was successful
  if (httpCode > 0) {
    // Check if the response indicates success
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println(response);
    } else {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    }
  } else {
    // Print the error if the request failed
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
}

// Create a payload for an HTTP POST request
String createPayload(String context) {
  String payload = "{\"event\":{\"value\":0,\"context\":{\"notification\":\"" + context + "\"}}}";
  return payload;
}

// Disconnect from the WiFi network
void disconnectWiFi(HTTPClient& http) {
  http.end();
}
