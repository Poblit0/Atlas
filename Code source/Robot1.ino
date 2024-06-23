#include <QTRSensors.h>
#include <SoftwareSerial.h>
#include "Servo.h"

//Pour les servomoteurs
Servo servo1;
Servo servo2;

//Pour la communication RF
SoftwareSerial HC12(11, 12);  
char robot = '0';
char msg;
char positionRobot1 = '0';

//Pour les capteurs
QTRSensors qtr;
const int nombreDeCapteurs = 8;
int valeursCapteurs[nombreDeCapteurs];

//Pour les moteurs
const int M1A = 3;
const int M2A = 6;
const int M1B = 5;
const int M2B = 9;
int vitesse = 100;

//Pour le correcteur
const double Kp = 0.023;     
const double Kd = 0.08;      
double erreurPrecedente = 0.0;
const int positionVoulue = 3500;

//Pour l'entrepot
char allerVersA[10] = {'A','G','T','G','C','G','G','D','D','A'};
char allerVersB[12] = {'A','G','T','T','G','C','G','T','G','D','D','A'};
char allerVersC[10] = {'A','G','T','D','C','D','D','G','D','A'};
char allerVersD[12] = {'A','G','T','T','D','C','D','T','D','G','D','A'};
char consigne[2] = {'N','N'};
char chemin[12];
int intersection = 0;

//Pour les rotations
int tempsRotation = 500;
long tempsPrecedent = 0;
long tempsActuel = 0;

void setup() {
  servo1.attach(2);
  servo2.attach(4);
  servo1.write(0);
  servo2.write(0); 
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){38, 40, 42, 44, 46, 48, 50, 52}, nombreDeCapteurs);
  pinMode(M1A, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2B, OUTPUT);
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Initialisation");
  delay(2000);
  Serial.println("Début calibrage");
  calibrageCapteurs();
  Serial.println("Fin calibrage");
}

void loop() {
  //Si le robot 1 n'a pas reçu de consigne
  if ((consigne[0] != 'R' && consigne[0] != 'E') || (consigne[1] != 'A' && consigne[1] != 'B' && consigne[1] != 'C' && consigne[1] != 'D') || robot != '1'){
    while (HC12.available()) {
      msg = HC12.read();
      //Choix de l'étagère (A,B,C ou D)
      if (msg == 'A' || msg == 'B' || msg == 'C' || msg == 'D') {    
        consigne[1] = msg;  
      }
      //Choix de la consigne (récupérer ou entreposer)
      if (msg == 'R' || msg == 'E'){
        consigne[0] = msg;
      }
      //Choix du robot (1 ou 2)
      if (msg == '1'){
        robot = msg;
      }
    }
    if (consigne[1] == 'A'){
      for (int i = 0; i < 10; i++){
        chemin[i] = allerVersA[i];
      }
    }
    if (consigne[1] == 'B'){
      for (int i = 0; i < 12; i++){
        chemin[i] = allerVersB[i];
      }
    }
    if (consigne[1] == 'C'){
      for (int i = 0; i < 10; i++){
        chemin[i] = allerVersC[i];
      }
    }
    if (consigne[1] == 'D'){
      for (int i = 0; i < 12; i++){
        chemin[i] = allerVersD[i];
      }
    }
    if ((consigne[0] == 'E') && (robot == '1')){
      leverPalette();
    }
  }
  else {
    unsigned int position = qtr.readLineBlack(valeursCapteurs);
    //Si les capteurs détectent une intersection
    if ((valeursCapteurs[0] == 1000 && valeursCapteurs[1] == 1000 && valeursCapteurs[2] == 1000 && valeursCapteurs[3] == 1000) || (valeursCapteurs[7] == 1000 && valeursCapteurs[4] == 1000 && valeursCapteurs[5] == 1000 && valeursCapteurs[6] == 1000)){
      intersection = intersection + 1;
      tempsPrecedent = millis();
      while (tempsActuel - tempsPrecedent < 100){
        tempsActuel = millis();
        avancer();
      }
      tempsActuel = 0;
      rotation();
      arret();
      positionRobot1 = '0' + intersection;
      HC12.write(positionRobot1);
    }
    //Suivi de ligne
    else {
      int erreur = positionVoulue - position;
      int correction = Kp * erreur + Kd * (erreur - erreurPrecedente);
      erreurPrecedente = erreur;
      analogWrite(M2B, constrain(vitesse - correction, 0, 255)); 
      analogWrite(M1B, constrain(vitesse + correction, 0, 255));
    }
  }  
}

void rotation(){
  if (chemin[intersection] == 'G'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < tempsRotation){
      tempsActuel = millis();
      rotationGauche();
    }
    tempsActuel = 0;    
  }
  if (chemin[intersection] == 'D'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < tempsRotation){
      tempsActuel = millis();
      rotationDroite();
    }
    tempsActuel = 0;    
  }
  if (chemin[intersection] == 'A'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < 2 * tempsRotation){ 
      tempsActuel = millis();
      rotationDroite();
    }
    tempsActuel = 0; 
    intersection = 0;
    consigne[0] = 'N';
    consigne[1] = 'N';
    servo1.write(0);  
    servo2.write(0);  
  }
  if (chemin[intersection] == 'C'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < 400){
      tempsActuel = millis();
      avancer();
    }
    tempsActuel = 0;
    arret();
    if(consigne[0] == 'E'){
      descendrePalette();
    }
    if(consigne[0] == 'R'){
      leverPalette();
    }
  }
  if (chemin[intersection] == 'T'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < 400){
      tempsActuel = millis();
      avancer();
    }
    tempsActuel = 0;
  }  
}

void calibrageCapteurs(){
  delay(500);
  for (int i = 0; i < 400; i++){
    qtr.calibrate();
  }
}

void rotationDroite() {
  analogWrite(M2B, 0);
  analogWrite(M1B, vitesse);
  analogWrite(M2A, vitesse);
  analogWrite(M1A, 0);
}

void rotationGauche() {
  analogWrite(M2B, vitesse);
  analogWrite(M1B, 0);
  analogWrite(M2A, 0);
  analogWrite(M1A, vitesse);
}

void arret() {
  analogWrite(M1A, 0);
  analogWrite(M2A, 0);
  analogWrite(M1B, 0);
  analogWrite(M2B, 0);
}

void avancer() {
  analogWrite(M2B, vitesse);
  analogWrite(M1B, vitesse);
  analogWrite(M2A, 0);
  analogWrite(M1A, 0);
}

void leverPalette(){
  for (int i = 1; i < 9; i++) {
    servo2.write(0 + i * 5);
    servo1.write(0 + i * 5);
    delay(500);
  }
}

void descendrePalette(){
  for (int i = 1; i < 9; i++) {
    servo2.write(40 - i * 5);
    servo1.write(40 - i * 5);
    delay(500);
  }
}
