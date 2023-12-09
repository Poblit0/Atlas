#include "Servo.h"

Servo servo1;
Servo servo2;

const int bouton = 2;   // on définit une autre constante de type entier
const int bouton2 = 3;  // on définit une autre constante de type entier
int val = 1;            // déclaration d’une variable globale qui mémorise l’état du bouton
int etat = 1;
int ancien_val = 1;

void setup() {
  pinMode(bouton, INPUT);   // l'I/O 7 est utilisée comme une sortie
  pinMode(bouton2, INPUT);  // l'I/O 7 est utilisée comme une sortie
  servo1.attach(7);
  servo2.attach(8);
  servo1.write(180);
  servo2.write(180);
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  val = digitalRead(bouton2);  // lecture de l’état de l’entrée 7
  if (digitalRead(bouton) == HIGH) {
    Serial.println("Bonne reception");
  }
  if (val == LOW) {
    go_up();
  } else {
    servo1.write(180);
    servo2.write(180);
  }
}

void go_up() {
  for (int i = 0; i < 9; i++) {
    servo1.write(180 - i * 5);
    servo2.write(180 - i * 5);
    delay(1000);
  }
}
