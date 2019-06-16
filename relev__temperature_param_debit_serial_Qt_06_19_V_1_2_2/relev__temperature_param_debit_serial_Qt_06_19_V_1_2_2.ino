/*

   @version : V_1_2_2
   @description :
   Redéfinition de la fonction de mise à jour du chrono pellet. 
   Probleme rencontré : le compte boucle incremente le chronoPellets. Le FLAG de MAJ aussi, a partir du debut (initializationChrono).
                        => le chrono est erroné.
   Solution : changer beginningPellets <=> previewsTime dans la fonction upDateChrono().                   
   @caractéristiques électriques :
   prescence de pellet devant l'IR <=> out = 0V
   absence de pellet devant l'IR <=> out = 3.6V
   @author : Antoine
   @date : 09 juin 2019
*/
//-----------------------------------------------------------------------------------------------------------------------------------------
// BIBLIOTEQUE
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <Streaming.h>
//-----------------------------------------------------------------------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------------------------------------------------------------------
//#define COMMENT               // On/Off commentaire sur moniteur serie /!\ conflit dans la communication serie quand on ouvre l'appli Qt !!!
#define spd 90                  // vitesse moteur
#define prescaler 100           // in fine, defini la resolution du chrono (p/r a sa maj)

#define pinInterruptNoDebit 2
#define In1Pin 4
#define In2Pin 5
#define pwmPin 9
//-----------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------------------------------------------------------------------
boolean lapsOn = false;
volatile boolean pellets = false;
volatile boolean initializationChrono = false;
volatile boolean majChrono = false;
volatile boolean irState = false;
int compteBoucle = 0;
int dataReceived[8] = {0};
int timeOpenMs = 0;
int timeOpen = 0;
int timeCloseMs = 0;
int timeClose = 0;
char timeOpenChar[4] = {0, 0, 0, '\0'};
char timeCloseChar[4] = {0, 0, 0, '\0'};
unsigned long currentTime = 0;
unsigned long startTimeClose = 0;
volatile unsigned long previewsTime = 0;
volatile unsigned long chronoPellets = 0;
volatile unsigned long beginningPellets = 0;
volatile unsigned long endPellets = 0;

//-----------------------------------------------------------------------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  //Paramétrage moteur
  pinMode(In1Pin, OUTPUT);
  pinMode(In2Pin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  //On demarre le process moteur à l'arret
  digitalWrite(In1Pin, LOW);
  digitalWrite(In2Pin, LOW);
  digitalWrite(pwmPin, LOW);
  //Initialisation de communication sur port Serie
  Serial.begin(9600);
  //Paramétrage de l'interruptions sur la broches 2
  attachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit), interruptNoDebit, CHANGE);
  irState = digitalRead(pinInterruptNoDebit);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
/*
   On commence par verifier si des data's sont arrivées sur le serial bufferd (max 64bytes).
   Si oui, on les convertis et les place dans les variables appropriées (timeOpen/timeClose).
*/
//-----------------------------------------------------------------------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  receiveSerialData();
  if (timeOpen > 0 || lapsOn) {
    //Serial << "irState = " << irState << endl;
    if (lapsOn) {
      compteBoucle++;
      if ((compteBoucle > prescaler) || majChrono) {     
        detachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit));
        upDateChrono();
        attachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit), interruptNoDebit, CHANGE);
        compteBoucle = 0;
      }
      if (chronoPellets > timeOpenMs) {
        #ifdef COMMENT
        Serial << "MAIN ChronoPellets" << chronoPellets << endl;
        #endif
        motorStop();
        lapsOn = false;
        startTimeClose = millis();
        chronoPellets = 0;
        initializationChrono = false;
        majChrono = false;
      }
    }
    else {
      currentTime = millis();
      if ((currentTime - startTimeClose) > timeCloseMs) {
        irState = digitalRead(pinInterruptNoDebit);
        if (!irState) {
          beginningPellets = millis();
          previewsTime = beginningPellets;   // ???
          initializationChrono = true;
        }
        motorForward();
        lapsOn = true;
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
// METHODES
//-----------------------------------------------------------------------------------------------------------------------------------------
/* methode : available :
   Get the number of bytes (characters) available for reading from the serial port.
   This is data that’s already arrived and stored in the serial receive buffer (which holds 64 bytes).

   On place les 8 premiers caracteres dans un tableau, on vide le buffer de caracteres restant.
   On appel ensuite la methode de traitement des datas recues de l'appli Qt (de config debit pellet via Qt sur un port serial.)
*/
void receiveSerialData()
{
  if (Serial.available() > 7) {
    for (int i = 0; i < 8; i++) {
      dataReceived[i] = Serial.read();
    }
    while ((Serial.read()) != -1); //on vide le buffer
#ifdef COMMENT
    Serial << "data reiceved = ";
    for (int i = 0; i < 8; i++) {
      Serial.print(dataReceived[i], DEC);
    }
    Serial << endl;
#endif
    convertDataReceived();
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
/* Methode de traitement des datas recues de l'appli Qt :
   remaque :
   Les données sont envoyée depuis l'application Qt cree selon le format :
    < OxxxCxxx >
   où,
      O = open
      C = close
      x = un chiffre (=> 000 < range < 999)

   on recupere et verifie le premier octet du tableau de type int[8] qui doit correspondre a un "O"
   dans table ASCII: (79)2 <=> "O".
   Si l'on ne recoit la valeur (79)2 dans le premier octet, c est que l on a une erreur de transmission/encodage.
   Ensuite, on fait pareil pour la 2ieme moitier du tableau contenant les infos sur le timeOff de la config du debit.
   on recupere et verifie le 5ieme octet du tableau de type int[8] qui doit correspondre a un "C"
   dans table ASCII: (67)2 <=> "C".
   Si l'on ne recoit la valeur (67)2 dans le 5ieme octet, c est que l on a une erreur de transmission/encodage.
*/
void convertDataReceived() {
  switch (dataReceived[0]) {

    case 79:
      for (int c = 1; c < 4; c++) {
        timeOpenChar[c - 1] = dataReceived[c]; //car le premier octet est la lettre..
      }
#ifdef COMMENT
      Serial << " time Open = " << timeOpenChar[0] << timeOpenChar[1] << timeOpenChar[2] << endl;
#endif
      // on converti un tableau de char[] en un "int".
      // /!\ prevoir un caractere de stop dans la tab char[] pour la converssion (\0) <=> timeOpenChar[4] = {0, 0, 0, '\0'}
      sscanf(timeOpenChar, "%d", &timeOpen);
      timeOpenMs = (timeOpen * 1000);
#ifdef COMMENT
      Serial << " time OPEN = " << timeOpen << endl;
#endif
      break;
    default:
#ifdef COMMENT
      Serial << "erreur \n Format incorrect" << endl;
#endif
      break;
  }
  switch (dataReceived[4]) {
    case 67:
      for (int c = 5; c < 8; c++) {
        timeCloseChar[c - 5] = dataReceived[c]; //car le premier octet est la lettre..
      }
#ifdef COMMENT
      Serial << " time Close = " << timeCloseChar[0] << timeCloseChar[1] << timeCloseChar[2] << endl;
#endif
      //delay(100);
      // on converti un tableau de char[] en un "int".
      // /!\ prevoir un caractere de stop dans la tab char[] pour la converssion (\0). <=> timeCloseChar[4] = {0, 0, 0, '\0'}
      sscanf(timeCloseChar, "%d", &timeClose);
      timeCloseMs = (timeClose * 1000);
#ifdef COMMENT
      delay(100);
      Serial << " time CLOSE = " << timeClose << endl;
#endif
      break;
    default:
#ifdef COMMENT
      Serial << "erreur \n Format incorrect" << endl;
#endif
      break;
  }
}


void motorStop() {
#ifdef COMMENT
  Serial << " STOP " << endl;
#endif
  digitalWrite(In1Pin, LOW);
  digitalWrite(In2Pin, LOW);
}

void motorForward() {
#ifdef COMMENT
  Serial << " marche_AVT " << endl;
#endif
  digitalWrite(In1Pin, HIGH);
  digitalWrite(In2Pin, LOW);
  analogWrite(pwmPin, spd);
}

void motorBackward() {
#ifdef COMMENT
  Serial << " marche_ARRIERE " << endl;
#endif
  digitalWrite(In1Pin, LOW);
  digitalWrite(In2Pin, HIGH);
  analogWrite(pwmPin, spd);
  // lapsOn ???
}

void upDateChrono() {
#ifdef COMMENT
  Serial << "MAJ chrono" << endl;
  Serial << "Compte boucle = " << compteBoucle << endl;
#endif
  if ((!majChrono) && (!irState)) {
    currentTime = millis();
    chronoPellets += (currentTime - previewsTime);
#ifdef COMMENT
  Serial << "via compteBoucle" << endl;
  Serial << "CurrentTime = " << currentTime << endl;
  Serial << "previewsTime = " << previewsTime << endl;
  Serial << "chronoPellets = " << chronoPellets << endl;
#endif
    previewsTime = currentTime;    
  }
  else if (majChrono && initializationChrono) {
    chronoPellets += (endPellets - previewsTime);
    majChrono = false;
#ifdef COMMENT
  Serial << "via MAJ FLAG (flux int)" << endl;  
  Serial << "beginningPellets = " << beginningPellets  << endl;
  Serial << "endPellets  = " << endPellets  << endl;
  Serial << "chronoPellets = " << chronoPellets << endl;
#endif
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// INTERRUPTIONS (Interupt Service Routine)
//-----------------------------------------------------------------------------------------------------------------------------------------

void interruptNoDebit() {
  if (lapsOn) {
    irState = digitalRead(pinInterruptNoDebit);
    if (!irState) {
      beginningPellets = millis();
      previewsTime = beginningPellets;
      initializationChrono = true;
    }
    else {
      endPellets = millis();
      majChrono = true;
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
