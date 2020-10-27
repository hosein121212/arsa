#include <arsalearn_v1.h>

void setup() {
}
void loop() {
  if (pin(key,u) == 1)
  {
   pin(led,high);
  }
  else
  {
    pin(led,low);
  }
}
