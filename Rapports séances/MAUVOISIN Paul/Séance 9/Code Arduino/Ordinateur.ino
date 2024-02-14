#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);  // HC-12 TX Pin, HC-12 RX Pin

// Définir la taille maximale de la liste warehouse
#define MAX_WAREHOUSE_SIZE 2

// Déclarer la liste warehouse
char warehouse[MAX_WAREHOUSE_SIZE] = { 'A', 'B' };  // Exemple de liste initialisée avec 'A' et 'B'

void setup() {
  Serial.begin(9600);
  print_warehouse();
}

void loop() {
  if (Serial.available() > 0) {
    char inputChar = Serial.read();  // Lire le caractère reçu depuis le port série

    // Vérifier si le caractère reçu est présent dans la liste warehouse
    bool charFound = false;
    for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
      if (warehouse[i] == inputChar) {
        HC12.write('R');
        if (i == 0) {
          HC12.write('A');
        }
        if (i == 1) {
          HC12.write('B');
        }
        charFound = true;
        warehouse[i] = 'V';  // Remplacer la lettre par 'V'
        break;
      }
    }

    // Si le caractère n'est pas dans la liste warehouse, l'ajouter à la liste
    if (!charFound) {
      for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
        if (warehouse[i] == 'V') {
          HC12.write('D');
          if (i == 0) {
            HC12.write('A');
          }
          if (i == 1) {
            HC12.write('B');
          }
          warehouse[i] = inputChar;  // Ajouter le caractère à la liste
          break;
        }
      }
    }

    print_warehouse();  // Afficher la liste warehouse sur le port série
  }

  delay(100);  // Attendre un court laps de temps
}

void print_warehouse() {
  Serial.print("Warehouse: ");
  for (int i = 0; i < MAX_WAREHOUSE_SIZE; i++) {
    Serial.print(warehouse[i]);
    Serial.print(" ");
  }
  Serial.println();
}
