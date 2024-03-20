#include <Pixy2.h>

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
  
  // Grab blocks!
  pixy.ccc.getBlocks();
  
  // Place blocks in the corresponding signature entry
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
  delay(60000);
}

// Count the number of pills in a given area
float countPills(float width, float height) {
  float area = width * height;
  int standard_area = 289; // 17 x 17
  int totalPills = floor(area/standard_area);
  if (totalPills < 1) { totalPills = 1; }
  return totalPills;
}
