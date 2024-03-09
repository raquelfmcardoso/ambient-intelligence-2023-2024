void setup() {
  Serial.begin(115200); // Make sure to set the baud rate to match your Python script
}

void loop() {
  if (Serial.available() > 0) {
    // Read data from the serial port
    String data = Serial.readStringUntil('\n');
    
    // Print received data back to the serial port
    Serial.println("Received: " + data);
  }
}
