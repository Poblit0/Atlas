#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);  // HC-12 TX Pin, HC-12 RX Pin

// Définir la taille maximale de la liste warehouse
#define MAX_WAREHOUSE_SIZE 2

// Déclarer la liste warehouse
char warehouse[MAX_WAREHOUSE_SIZE] = { 'C', 'T' };

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  printWarehouse();
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
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
      HC12.write('D');
      if (i == 0) {
        HC12.write('A');
      }
      if (i == 1) {
        HC12.write('B');
      }
      break;
    }
  }
}