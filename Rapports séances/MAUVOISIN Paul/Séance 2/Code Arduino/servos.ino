#include "Servo.h"

Servo servo1;
Servo servo2;

void setup() {
  servo1.attach(7);
  servo2.attach(8);
  servo1.write(180);
  servo2.write(180);
  delay(1000);
  // for (int i = 0; i < 9; i++) {
  //   servo1.write(i * 5);
  //   servo2.write(i * 5);
  //   delay(1000);
  // }
}

void loop() {
  for (int i = 0; i < 9; i++) {
    servo1.write(180 - i * 5);
    servo2.write(180 - i * 5);
    delay(1000);
  }
}