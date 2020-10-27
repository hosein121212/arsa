#include <arsalearn_v1.h>

void setup() {
  run();
}
void loop() {

    int i =Serial.read();
if(i=='b')  pin(led,low);
 
  if (i=='a') pin(led,high);
}
