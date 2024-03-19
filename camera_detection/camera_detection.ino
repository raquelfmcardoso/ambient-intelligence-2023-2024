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

// This is the main Pixy object 
Pixy2 pixy;

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting...\n");
  pixy.init();
}

void loop()
{ 
  int i;
  int sigCounts[4] = {0}; // sigCounts[0] never used because sigs start on 1
  
  // grab blocks!
  pixy.ccc.getBlocks();
  
  // place blocks in the corresponding signature entry
  for (i=0; i<pixy.ccc.numBlocks; i++) {
    float width = pixy.ccc.blocks[i].m_width;
    float height = pixy.ccc.blocks[i].m_height;
    int sig = pixy.ccc.blocks[i].m_signature;
    int totalPills = countPills(width, height);
    sigCounts[sig] += int(totalPills);
    if (sigCounts[sig] > 4) sigCounts[sig] = 4;
  }

  // Print the counts for each signature
  for (int sig = 1; sig <= 3; sig++) {
    Serial.print(sigCounts[sig]);
    Serial.print(" ");
  }
  
  Serial.println("");
  delay(20000);
}

float countPills(float width, float height) {
  float area = width * height;
  int standard_area = 289;
  //Serial.println("area: " + String(area));
  int totalPills = floor(area/standard_area);
  if (totalPills < 1) { totalPills = 1; }
  return totalPills;
}
