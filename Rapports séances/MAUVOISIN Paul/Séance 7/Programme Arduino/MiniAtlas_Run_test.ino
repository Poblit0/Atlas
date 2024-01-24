//                                    //                           //
//                                    // DECLARATION DES VARIABLES //
//                                    //                           //

//Assignements aux PIN
//Pour les capteurs IR numeriques
const int pin_capteur1 = 13;
const int pin_capteur2 = 12;
const int pin_capteur3 = 11;
const int pin_capteur4 = 10;
const int pin_capteur5 = 8;
//Pour les moteurs
const int M1A = 3;
const int M2A = 6;
const int M1B = 5;
const int M2B = 9;
//Pour les capteurs IR analogiques
const int G_dig__capteur = 14;
const int D_dig_capteur = 15;
const int G_ana__capteur = A4;
const int D_ana_capteur = A5;

//Variables des vitesses de moteurs
int low_speed = 40;
int high_speed = 80;

//Variables de detection des capteurs numeriques
int valeur_capteur1 = 0;
int valeur_capteur2 = 0;
int valeur_capteur3 = 0;
int valeur_capteur4 = 0;
int valeur_capteur5 = 0;

//Variables de detection des capteurs analogiques
int ana_capteur_G;
int num_capteur_G;
int ana_capteur_D;
int num_capteur_D;
bool read_analog_sensor = false;

//Variables pour les intersections
int step = -1;
bool go_A_or_B = false;                                                //Si false on veut aller en A, si true on veut aller en B
char go_A[8] = { 'L', 'R', 'R', 'C', 'R', 'R', 'A', 'A' };             //'L' pour Left, 'R' pour Right, 'S' pour Straight, 'A' pour Accueil, 'C' pour Colis
char go_B[10] = { 'L', 'R', 'S', 'R', 'C', 'R', 'S', 'R', 'A', 'A' };  //'L' pour Left, 'R' pour Right, 'S' pour Straight, 'A' pour Accueil, 'C' pour Colis
bool change_step = false;
bool read_currentTime = true;
int currentTime = 0;
bool former_sensor_color = false;
int cpt_switch_color = 0;


//                                    //                //
//                                    // FONCTION SETUP //
//                                    //                //


void setup() {

  //Les capteurs numeriques sont INPUT
  pinMode(pin_capteur1, INPUT);
  pinMode(pin_capteur2, INPUT);
  pinMode(pin_capteur3, INPUT);
  pinMode(pin_capteur4, INPUT);
  pinMode(pin_capteur5, INPUT);

  //Les moteurs sont OUTPUT
  pinMode(M1A, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2B, OUTPUT);

  //Les capteurs analogiques sont INPUT
  pinMode(G_dig__capteur, INPUT);
  pinMode(D_dig_capteur, INPUT);
  pinMode(G_ana__capteur, INPUT);
  pinMode(D_ana_capteur, INPUT);

  //Démarrage Serial
  Serial.begin(9600);
  Serial.println("Initialisation");
  Serial.print("Step : ");
  Serial.println(step);

  delay(3000);
}


//                                    //               //
//                                    // FONCTION LOOP //
//                                    //               //


void loop() {
  global_detection();

  if (true_intersection() == true) {
    stop();
    delay(1000);
    if (change_step == true) {
      analog_sensor_detection();
      change_step = false;
      // if (analog_sensor == true) {
      //   stop();
      //   change_step = false;
      //   step += 1;
      //   Serial.print("Step : ");
      //   Serial.println(step);
      // }
      // if (analog_sensor == false) {
      //   forward();
      // }
    }
    //mapping();
  }

  else {
    change_step = true;
    simple_line();
  }
}


//                                    //                               //
//                                    // FONCTION POUR LA CARTOGRAPHIE //
//                                    //                               //


void mapping() {

  if (go_A_or_B == false) {
    if (go_A[step] == 'S') {
      forward();
    }
    if (go_A[step] == 'R') {
      intersection_rotate_right();
    }
    if (go_A[step] == 'L') {
      intersection_rotate_left();
    }
    if (go_A[step] == 'C') {
      stop();
      delay(2000);
      forward();
    }
    if (go_A[step] == 'A') {
      stop();
      delay(100000);
      //accueil();
    }
  }

  if (go_A_or_B == true) {
    if (go_B[step] == 'S') {
      forward();
    }
    if (go_B[step] == 'R') {
      intersection_rotate_right();
    }
    if (go_B[step] == 'L') {
      intersection_rotate_left();
    }
    if (go_B[step] == 'C') {
      stop();
      delay(2000);
      forward();
    }
    if (go_B[step] == 'A') {
      stop();
      delay(100000);
      //accueil();
    }
  }

  delay(350);
}


//                                    //                                   //
//                                    // FONCTION DE SUIVI DE LIGNE SIMPLE //
//                                    //                                   //


void simple_line() {
  if (valeur_capteur2 == LOW) {
    turn_left();
  }

  else if (valeur_capteur4 == LOW) {
    turn_right();
  }

  else {
    forward();
  }
}


//                                    //                       //
//                                    // FONCTION INTERSECTION //
//                                    //                       //


bool true_intersection() {
  if ((valeur_capteur1 == HIGH) && (valeur_capteur5 == HIGH)) {
    read_currentTime = true;
    return false;
  }
  if ((valeur_capteur1 == LOW) || (valeur_capteur5 == LOW)) {
    if (read_currentTime == true) {
      read_currentTime = false;
      currentTime = millis();
    }
    if ((millis() - currentTime) > 100) {
      return true;
    }
  }
}


void analog_sensor_detection() {
  analog_detection();
  if ((num_capteur_G == 0) && (num_capteur_D == 0)) {
    read_analog_sensor = true;
  }
  if (read_analog_sensor == true) {
    if ((num_capteur_G == 1) || (num_capteur_D == 1)) {
      stop();
      delay(9000);
    }
  }
  else {
    forward();
  }
}


//                                    //                              //
//                                    // FONCTION DE ROTATION ACCUEIL //
//                                    //                              //


void accueil() {
  if (read_currentTime == true) {
    read_currentTime = false;
    currentTime = millis();
  }
  if ((millis() - currentTime) > 1000) {
    while ((valeur_capteur2 == HIGH) && (cpt_switch_color >= 2)) {
      self_rotate();
    }
  }
}


//                                    //                                    //
//                                    // FONCTION D'ACTIVATION DES CAPTEURS //
//                                    //                                    //


void global_detection() {
  valeur_capteur1 = digitalRead(pin_capteur1);
  valeur_capteur2 = digitalRead(pin_capteur2);
  valeur_capteur3 = digitalRead(pin_capteur3);
  valeur_capteur4 = digitalRead(pin_capteur4);
  valeur_capteur5 = digitalRead(pin_capteur5);
}

void analog_detection() {
  ana_capteur_G = analogRead(G_ana__capteur);
  num_capteur_G = digitalRead(G_dig__capteur);
  ana_capteur_D = analogRead(D_ana_capteur);
  num_capteur_D = digitalRead(D_dig_capteur);
  // Serial.print(" Analogue = ");
  // Serial.print(ana_capteur_G);
  // Serial.print(" Analogue = ");
  // Serial.print(ana_capteur_D);
  // Serial.print(" Digital =");
  // Serial.print(num_capteur_G);
  // Serial.print(" Digital =");
  // Serial.println(num_capteur_D);
}

//                                    //                                   //
//                                    // FONCTION DE CHANGEMENT DE COULEUR //
//                                    //                                   //

void switch_color() {
  if (valeur_capteur4 != former_sensor_color) {
    if ((valeur_capteur4 == HIGH)) {
      cpt_switch_color += 1;  //Du noir vers le blanc
    }
    if ((valeur_capteur4 == LOW) && (former_sensor_color = true)) {
      cpt_switch_color += 1;  //Du blanc vers le noir
    }
    former_sensor_color = valeur_capteur4;
  }
}


//                                    //                               //
//                                    // FONCTIONS POUR LE DEPLACEMENT //
//                                    //                               //


void stop() {
  low_speed = 0;
  analogWrite(M1A, 0);
  analogWrite(M2A, 0);
  analogWrite(M1B, low_speed);
  analogWrite(M2B, low_speed);
}

void forward() {
  analogWrite(M1A, 0);
  analogWrite(M2A, 0);
  analogWrite(M1B, high_speed);
  analogWrite(M2B, high_speed);
}

void turn_right() {
  analogWrite(M1A, 0);
  analogWrite(M2A, 0);
  analogWrite(M1B, high_speed);
  analogWrite(M2B, low_speed);
}

void turn_left() {
  analogWrite(M1A, 0);
  analogWrite(M2A, 0);
  analogWrite(M1B, low_speed);
  analogWrite(M2B, high_speed);
}

void self_rotate() {
  analogWrite(M1A, low_speed);
  analogWrite(M2A, 0);
  analogWrite(M1B, 0);
  analogWrite(M2B, low_speed);
}

void intersection_rotate_right() {
  analogWrite(M1A, 0);
  analogWrite(M2A, low_speed);
  analogWrite(M1B, high_speed);
  analogWrite(M2B, 0);
}

void intersection_rotate_left() {
  analogWrite(M1A, low_speed);
  analogWrite(M2A, 0);
  analogWrite(M1B, 0);
  analogWrite(M2B, high_speed);
}
