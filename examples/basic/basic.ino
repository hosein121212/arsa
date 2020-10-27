#include <arsalearn_v1.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}
int e = 0;
int i = 0;
void loop() {

  // put your main code here, to run repeatedly:
  pin(led2,high);
  delay(200);
  pin(led2 ,low);
  delay(200);
  Serial.println(i);
  if (pin(key4) == 1)
  {
    i++;
  }

}
