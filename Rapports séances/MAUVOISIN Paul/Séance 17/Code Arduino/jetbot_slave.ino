#include <Wire.h>
#include "Servo.h"

Servo servo1;

#define SLAVE_ADDRESS 0x04
#define IR_SENSOR_PIN 2

bool state;

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  pinMode(IR_SENSOR_PIN, INPUT);
  Serial.begin(9600);

  servo1.attach(3);

  servo1.write(100);

  delay(1000);
}

void loop() {
  state = digitalRead(IR_SENSOR_PIN);  // Read the state of the IR sensor
  Serial.println(state);

  if (state == LOW) {
    for (int i = 0; i < 9; i++) {
      servo1.write(180 - i * 5);

      delay(1000);
    }
  }
}

void requestEvent() {
  state = digitalRead(IR_SENSOR_PIN);  // Read the state of the IR sensor
  if (state == 0) {
    Wire.write(false);
  }
  if (state == 1) {
    Wire.write(true);  // Send state as a byte (0 for false, 1 for true)
  }
}
