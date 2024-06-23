#include <QTRSensors.h>
#include <SoftwareSerial.h>
#include "Servo.h"

//Pour les servomoteurs
Servo servo1;
Servo servo2;

//Pour la communication RF
SoftwareSerial HC12(11, 12);  
char msg;
bool positionR1_recue = false;
int positionRobot1 = 0;

//Pour les capteurs
QTRSensors qtr;
const int nombreDeCapteurs = 8;
int valeursCapteurs[nombreDeCapteurs];

//Pour les moteurs
const int M1A = 3;
const int M2A = 6;
const int M1B = 5;
const int M2B = 9;
int vitesse = 80;

//Pour le correcteur
const double Kp = 0.01;
const double Kd = 0.5;
double erreurPrecedente = 0.0;
const int positionVoulue = 3500;

//Pour l'entrepot
char allerVersA[10] = {'A','D','T','G','C','G','G','D','G','A'}; 
char allerVersB[12] = {'A','D','T','T','G','C','G','T','G','D','G','A'};
char allerVersC[10] = {'A','D','T','D','C','D','D','G','G','A'};
char allerVersD[12] = {'A','D','T','T','D','C','D','T','D','G','G','A'};
char chemin[12];
char consigneRobot[2] = {'N','N'};
char consigneRobot1 = 'N';
char robot = '0';
int intersection = 0;

//Pour les rotations
long tempsPrecedent = 0;
long tempsActuel = 0;
int tempsRotation= 350;

void setup() {
  servo1.attach(2);
  servo2.attach(4);
  servo1.write(20);  
  servo2.write(5);
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){38, 40, 42, 44, 46, 48, 50, 52}, nombreDeCapteurs); 
  pinMode(M1A, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2B, OUTPUT);
  HC12.begin(9600);
  Serial.begin(9600);
  Serial.println("Initialisation");
  delay(2000);
  Serial.println("Début calibrage");
  calibrageCapteurs();
  Serial.println("Fin calibrage");
}

void loop() {
  //Si le robot 2 n'a pas reçu de consigne
  if ((consigneRobot[0] != 'R' && consigneRobot[0] != 'E') || (consigneRobot[1] != 'A' && consigneRobot[1] != 'B' && consigneRobot[1] != 'C' && consigneRobot[1] != 'D') || robot != '2'){
    if (HC12.available()) {
      msg = HC12.read();
      //Choix du robot (1 ou 2)
      if (msg == '1' || msg == '2'){
        robot = msg;
      }
      //Choix de l'étagère (A,B,C ou D)
      if (msg == 'A' || msg == 'B' || msg == 'C' || msg == 'D') {    
        consigneRobot[1] = msg;  
      }
      //Choix de la consigne (récupérer ou entreposer)
      if (msg == 'R' || msg == 'E'){
        consigneRobot[0] = msg;
      }
      //Consigne du robot 1
      if (robot == '1'){
        consigneRobot1 = consigneRobot[1];
        consigneRobot[0] = 'N';
        consigneRobot[1] = 'N';
      }   
    }
    if (consigneRobot[1] == 'A'){
      for(int i = 0; i < 10; i++){
        chemin[i] = allerVersA[i];
      }
    }
    if (consigneRobot[1] == 'B'){
      for(int i = 0; i<12; i++){
        chemin[i] = allerVersB[i];
      }
    }
    if (consigneRobot[1] == 'C'){
      for(int i = 0; i < 10; i++){
        chemin[i] = allerVersC[i];
      }
    }
    if (consigneRobot[1] == 'D'){
      for(int i = 0; i < 12; i++){
        chemin[i] = allerVersD[i];
      }
    }
  }
  else {
    //Si le robot est à l'accueil
    if (intersection == 0){
      if(consigneRobot[0] == 'E'){
        leverPalette();
      }
      //Tant que les robots sont à proximité l'un de l'autre et risquent d'entrer en collision, le robot 2 n'avance pas
      while (positionRobot1 == 0 || positionRobot1 == 1 || ((positionRobot1 == 5 || positionRobot1 == 6 || positionRobot1 == 7) && (consigneRobot1 == 'A' || consigneRobot1 == 'C')) || ((positionRobot1 == 7 || positionRobot1 == 8 || positionRobot1 == 9) && (consigneRobot1 == 'B' || consigneRobot1 == 'D'))){
        arret();
        if (HC12.available()) {
          msg = HC12.read();
          positionRobot1 = msg - '0';
        }
      } 
    }
    unsigned int position = qtr.readLineBlack(valeursCapteurs);
    //Si les capteurs détectent une intersection
    if ((valeursCapteurs[0] == 1000 && valeursCapteurs[1] == 1000 && valeursCapteurs[2] == 1000 && valeursCapteurs[3] == 1000) || (valeursCapteurs[7] == 1000 && valeursCapteurs[4] == 1000 && valeursCapteurs[5] == 1000 && valeursCapteurs[6] == 1000)){
      intersection = intersection + 1;
      tempsPrecedent = millis();
      while(tempsActuel - tempsPrecedent < 100){
        tempsActuel = millis();
        avancer();
      }
      tempsActuel = 0;
      rotation();
      arret();
      //Si le robot 2 se situe aux intersections 2,3,4,6 ou 8, il recoit la position du robot 1
      if (intersection == 2 || intersection == 3 || intersection == 4 || intersection == 6 || intersection == 8) {
        positionR1_recue = false;
        while(positionR1_recue == false){
          if (HC12.available()) {
            msg = HC12.read();
            positionRobot1 = msg - '0';
            positionR1_recue = true;
          }
        }
        //Tant que les robots sont à proximité l'un de l'autre et risquent d'entrer en collision, le robot 2 n'avance pas
        while ((intersection == 2 && (consigneRobot[1] == 'A' || consigneRobot[1] == 'C') && (consigneRobot1 == 'A' || consigneRobot1 == 'C') && (positionRobot1 == 3 || positionRobot1 == 4)) || (intersection == 3 && (consigneRobot[1] == 'B' || consigneRobot[1] == 'D') && (consigneRobot1 == 'B' || consigneRobot1 == 'D') && (positionRobot1 == 4 || positionRobot1 == 5)) || (intersection == 4 && ((consigneRobot[1] == 'A' && consigneRobot1 == 'B') || (consigneRobot[1] == 'C' && consigneRobot1 == 'D')) && positionRobot1 == 6) || (((intersection == 6 && (consigneRobot[1] == 'A' || consigneRobot[1] == 'C')) || (intersection == 8 && (consigneRobot[1] == 'B' || consigneRobot[1] == 'D'))) && ((positionRobot1 == 6 && (consigneRobot1 == 'A' || consigneRobot1 == 'C')) || (positionRobot1 == 8 && (consigneRobot1 == 'B' || consigneRobot1 == 'D'))))){
          arret();
          if (HC12.available()) {
            msg = HC12.read();
            positionRobot1 = msg - '0';
          }
        }
      }
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
    consigneRobot[0] = 'N';
    consigneRobot[1] = 'N';
    servo1.write(20);  
    servo2.write(5);  
  }
  if (chemin[intersection] == 'C'){
    tempsPrecedent = millis();
    while(tempsActuel - tempsPrecedent < 400){
      tempsActuel = millis();
      avancer();
    }
    tempsActuel = 0;
    arret();
    if(consigneRobot[0] == 'E'){
      descendrePalette();
    }
      if(consigneRobot[0] == 'R'){
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

void calibrageCapteurs(){
  delay(500);
  for (int i = 0; i < 400; i++){
    qtr.calibrate();
  }
}

void leverPalette(){
  for (int i = 1; i < 9; i++) {
    servo2.write(5 + i * 5);
    servo1.write(20 + i * 5);
    delay(500);
  }
}

void descendrePalette(){
  for (int i = 1; i < 9; i++) {
    servo2.write(45 - i * 5);
    servo1.write(60 - i * 5);
    delay(500);
  }
}
