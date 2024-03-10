//#define PIN_RED    D11 // The Arduino Nano ESP32 pin connected to R pin
//#define PIN_GREEN  D10 // The Arduino Nano ESP32 pin connected to G pin
//#define PIN_BLUE   D9  // The Arduino Nano ESP32 pin connected to B pin

const int MAX_VARIABLES = 9; // Maximum number of VariableInfo objects

void setup() {
  Serial.begin(115200); // Make sure to set the baud rate to match your Python script
  //pinMode(PIN_RED,   OUTPUT);
  //pinMode(PIN_GREEN, OUTPUT);
  //pinMode(PIN_BLUE,  OUTPUT);
}

struct VariableInfo {
  const char* label;
  const char* timestamp;
  int value;
};

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
      data.label = label.c_str();
      data.timestamp = timestamp.c_str();
      data.value = intValue;

      // Store the VariableInfo object in the array
      variableData[i] = data;

      // Print received data back to the serial port
      Serial.println("Received Label: " + label);
      Serial.println("Received Timestamp: " + timestamp);
      Serial.println("Received Value: " + value);
    }
  }
}
