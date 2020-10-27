
#include <arsalearn_v1.h>// کتابخانه جامع آرسا                 
#include <arsa_blue_v1.h>// کتابخانه بلوتوث                 
arsa_blue_v1 BT;
void setup() {
  Serial.begin(115200);
  BT.begin("alireza"); //يک نام براي بلوتوث خود بنويسيد        
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  
  BT.en();
}
