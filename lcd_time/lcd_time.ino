#include <LiquidCrystal.h> // Include the LiquidCrystal library
#include <Time.h> // Include the Time library
#include <TimeLib.h> // Include the TimeLib library

// Define LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Function prototype
void displayDateTime();

void setup() {
  // Set initial time (hour, minute, second, day, month, year)
  setTime(0, 10, 0, 17, 3, 2024);

  // Initialize LCD
  lcd.begin(16, 2);
}

void loop() {
  lcd.clear(); // Clear the LCD screen
  displayDateTime(); // Display date and time
  delay(1000); // Delay for one second
}

// Function to display date and time
void displayDateTime() {
  // Print time (hours and minutes)
  lcd.setCursor(5, 0); // Set cursor position for time

  String hourStr = String(hour(), DEC); // Convert hour to string with decimal base
  if (hour() < 10) {
    hourStr = "0" + hourStr; // Prepend "0" if hour is less than 10
  }

  String minuteStr = String(minute(), DEC); // Convert minute to string with decimal base
  if (minute() < 10) {
    minuteStr = "0" + minuteStr; // Prepend "0" if minute is less than 10
  }

  lcd.print(hourStr + ":" + minuteStr);

  // Print date (day, month, and year)
  lcd.setCursor(3, 1); // Set cursor position for date
  lcd.print(String(day()) + "/" + String(month()) + "/" + String(year()));
}