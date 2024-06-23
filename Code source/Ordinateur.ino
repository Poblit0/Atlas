#include <SoftwareSerial.h>

SoftwareSerial HC12(7, 8);  // HC-12 TX Pin, HC-12 RX Pin

// Définir la taille maximale de la liste warehouse
#define MAX_WAREHOUSE_SIZE 4

// Déclarer la liste warehouse
char warehouse[MAX_WAREHOUSE_SIZE] = {'C','T','R','A'};

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  printWarehouse();
}

void loop() {
  if (Serial.available() >= 2) {
    char robot = Serial.read();
    char input = Serial.read();
    HC12.write(robot);
    if (input != '\n' && input != '\r' && input != 'V') {
      boolean found = false;
      for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
        if (warehouse[i] == input) {
          warehouse[i] = 'V';
          found = true;
          HC12.write('R');
          if (i == 0) {
            HC12.write('A');
          }
          if (i == 1) {
            HC12.write('B');
          }
          if (i == 2) {
            HC12.write('C');
          }
          if (i == 3) {
            HC12.write('D');
          }
          break;
        }
      }
      if (!found && hasEmptySlot() && input != 'V') {
        replaceFirstEmptySlot(input);
      }
      printWarehouse();
    }
  }
}

void printWarehouse() {
  Serial.println("Warehouse:");
  Serial.print("[ ");
  for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
    Serial.print(warehouse[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  Serial.println();
}

boolean hasEmptySlot() {
  for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
    if (warehouse[i] == 'V') {
      return true;
    }
  }
  return false;
}

void replaceFirstEmptySlot(char input) {
  for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
    if (warehouse[i] == 'V') {
      warehouse[i] = input;
      HC12.write('E');
      if (i == 0) {
        HC12.write('A');
      }
      if (i == 1) {
        HC12.write('B');
      }
      if (i == 2) {
        HC12.write('C');
      }
      if (i == 3) {
        HC12.write('D');
      }
      break;
    }
  }
}