#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);  // HC-12 TX Pin, HC-12 RX Pin

char msg;
bool brain[2];

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Initialisation Robot");
}

void loop() {
  while (HC12.available()) {
    msg = HC12.read();
    Serial.println(msg);
  }

  if (msg == 'D') {    //Char 'D' pour déposer
    brain[0] = false;  //Lorsque le robot ira voir dans sa liste l'action qu'il doit faire il saura qu'il doit déposer le colis
  }
  if (msg == 'R') {   //Char 'R' pour récupérer
    brain[0] = true;  //Lorsque le robot ira voir dans sa liste l'action qu'il doit faire il saura qu'il doit récupérer le colis
  }


  if (msg == 'A') {    //Char 'A' pour l'étagère A
    brain[1] = false;  //Lorsque le robot ira voir dans sa liste l'action qu'il doit faire il saura qu'il doit aller en A
  }
  if (msg == 'B') {   //Char 'B' pour l'étagère B
    brain[1] = true;  //Lorsque le robot ira voir dans sa liste l'action qu'il doit faire il saura qu'il doit aller en B
  }
}