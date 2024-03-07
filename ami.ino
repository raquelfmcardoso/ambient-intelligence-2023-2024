//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//
// This sketch is a good place to start if you're just getting started with 
// Pixy and Arduino.  This program simply prints the detected object blocks 
// (including color codes) through the serial console.  It uses the Arduino's 
// ICSP SPI port.  For more information go here:
//
// https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:hooking_up_pixy_to_a_microcontroller_-28like_an_arduino-29
//
  
#include <Pixy2.h>
#include <Time.h>
#include <TimeLib.h>

// This is the main Pixy object 
Pixy2 pixy;

struct SignatureInfo {
  int id;
  const char* name;
};

SignatureInfo signatureMap[] = {
  {1, "Orange"},
  {2, "Blue"},
  {3, "Green"},
  {4, "Purple"},
};

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting...\n");
  pixy.init();
  setTime(23,40,0,7,3,24);
}

void loop()
{ 
  int i;
  int sigCounts[5] = {0};

  Serial.println("----");
  digitalClockDisplay();
  // grab blocks!
  pixy.ccc.getBlocks();
  
  // If there are detect blocks, print them!
  if (pixy.ccc.numBlocks)
  {
    for (i=0; i<pixy.ccc.numBlocks; i++)
    {
      int sig = pixy.ccc.blocks[i].m_signature;
      sigCounts[sig]++;
    }

    // Print the counts for each signature
    for (int sig = 1; sig <= 4; sig++) {
      Serial.print(signatureMap[sig-1].name); // starts on 0
      Serial.print(": ");
      Serial.println(sigCounts[sig]);
    }
  }
  delay(60000);
}

void digitalClockDisplay()
{
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();  
}

void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

