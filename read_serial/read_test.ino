//#define PIN_RED    D11 // The Arduino Nano ESP32 pin connected to R pin
//#define PIN_GREEN  D10 // The Arduino Nano ESP32 pin connected to G pin
//#define PIN_BLUE   D9  // The Arduino Nano ESP32 pin connected to B pin
#include <WiFi.h>
#include "time.h"
#include "pitches.h"
#define PIN_RGB    D2 
#define PIN_2COLOR  D3 
#define PIN_2CMINI   D4  

#define BUZZZER_PIN D5 // The Arduino Nano ESP32 pin connected to piezo buzzer

// notes in the melody:
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

const int MAX_VARIABLES = 9; // Maximum number of VariableInfo objects

const char* ssid     = "Vodafone-F81A50-Plus";
const char* password = "RF4BCz3Dzn";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

unsigned long buzzerStartTime = 0; // Variable to store the start time of the buzzer
const unsigned long buzzDuration = 20000; // 20 seconds in milliseconds

struct HourMinute {
    int hour;
    int minute;
};

struct VariableInfo {
  String label;
  String timestamp;
  int value;
  //int previousValue; (idunno how to get it)
};

// Pins corresponding to each medication time
const int medPresPins[3] = {PIN_RGB, PIN_2COLOR, PIN_2CMINI};

HourMinute medPres[3]; // Array to hold the medication times
int medInventory[3] = {0};

void setup() {
  Serial.begin(115200); // Make sure to set the baud rate to match your Python script
  //pinMode(PIN_RED,   OUTPUT);
  //pinMode(PIN_GREEN, OUTPUT);
  //pinMode(PIN_BLUE,  OUTPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

/* void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
 
  strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
  Serial.println("Current time: " + String(timeString));
} */

HourMinute getLocalHourMinute() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return {0, 0}; // Return 0 values if failed to obtain time
  }

  HourMinute localTime;
  localTime.hour = timeinfo.tm_hour;
  localTime.minute = timeinfo.tm_min;

  return localTime;
}

void printLocalTime() {
  HourMinute time = getLocalHourMinute();
  
  Serial.print("Hour: ");
  Serial.println(time.hour);
  Serial.print("Minute: ");
  Serial.println(time.minute);
}

void buzz() {
  /// no need to repeat the melody.
  int size = sizeof(noteDurations) / sizeof(int);

  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // The note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
}

void stopBuzz() {
  noTone(BUZZZER_PIN);
}

void loop() {
  //analogWrite(PIN_RED,   0);
  //analogWrite(PIN_GREEN, 201);
  //analogWrite(PIN_BLUE,  204);

  VariableInfo variableData[MAX_VARIABLES]; // Array of VariableInfo objects

  for (int i = 0; i < MAX_VARIABLES; i++) {
    if (Serial.available() > 0) {
      // Read data from the serial port
      String information = Serial.readStringUntil('\n');

      // Find the positions of the delimiters
      int comma1 = information.indexOf(',');
      int comma2 = information.indexOf(',', comma1 + 1);

      // Extract substrings
      String label = information.substring(0, comma1);
      String timestamp = information.substring(comma1 + 1, comma2);
      String value = information.substring(comma2 + 1);

      // Convert value to integer
      int intValue = value.toInt();

      // Create a new VariableInfo object
      VariableInfo data;
      data.label = label;
      data.timestamp = timestamp;
      data.value = intValue;

      // Store the VariableInfo object in the array
      variableData[i] = data;

      // Print received data back to the serial port
      Serial.println("Received Label: " + variableData[i].label);
      Serial.println("Received Timestamp: " + variableData[i].timestamp);
      Serial.println("Received Value: " + String(variableData[i].value));

      //TOTALK:im so sorry this is so hardcoded
      if (i == 3 || i == 5 || i == 7) {
        HourMinute time;
        time.hour = variableData[i].value;
        time.minute = variableData[i+1].value;
      
        medPres[(i-3)/2] = time;
      }

      if (i == 0 || i == 1 || i == 2) {
        medInventory[i] = variableData[i].value;
        medPreviousValue[i] = variableData[i].previousValue;
      }

      //Serial.println("Variable Data label: " + variableData[i].label);
      //Serial.println("Variable Data timestamp: " + variableData[i].timestamp);
      //Serial.println("Variable Data value: " + variableData[i].value);

    }
  }

  //delay(1000);
  //printLocalTime();

  for (int i = 0; i < 3 ; i++) {
    if (getLocalHourMinute().hour == medPres[i].hour && getLocalHourMinute().minute == medPres[i].minute) {
      Serial.println("Time to take medicine " + String(i+1));
      digitalWrite(medPresPins[i], HIGH);
      buzz();

      if (medInventory[i] == previousValue[i] - 1) {
        Serial.println("Medicine taken");
        digitalWrite(medPresPins[i], LOW);
        stopBuzz();
      }
      else if (medInventory[i] == previousValue[i]) {
        Serial.println("Medicine not taken");
        buzzerStartTime = millis(); // Record the start time of the buzzer
        //digitalWrite(medPresPins[i], HIGH);
        //buzz(); 

        // Wait for 10 seconds or until inventory changes
        while ((millis() - buzzerStartTime < buzzDuration) && (medInventory[i] == previousValue[i])) {
          // Check if inventory has changed
          // Read from the serial port for any new data

          //TOTALK: omg send help
          // if (Serial.available() > 0) {
          //   // Read data from the serial port
          //   String information = Serial.readStringUntil('\n');

          //   // Find the positions of the delimiters
          //   int comma1 = information.indexOf(',');
          //   int comma2 = information.indexOf(',', comma1 + 1);

          //   // Extract substrings
          //   String label = information.substring(0, comma1);
          //   String timestamp = information.substring(comma1 + 1, comma2);
          //   String value = information.substring(comma2 + 1);

          //   // Convert value to integer
          //   int intValue = value.toInt();

          //   // Update inventory
          //   medInventory[i] = intValue;
          // }
        }

        // After 10 seconds or inventory change, stop the buzzer and turn off the LED
        stopBuzz();
        digitalWrite(medPresPins[i], LOW);
      }
      else if (medInventory[i] == 0) {
        Serial.println("No medicine in inventory");
        digitalWrite(medPresPins[i], LOW);
        stopBuzz();
      }
    }
   }
}

