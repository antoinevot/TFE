/*
   @version : V_2_3_0
   @description :
   Ajout de la transmission des données "time" & "temp" a sauvegarder dans un fichier via l'appli crée avec Qt.
   @caractéristiques électriques :
   prescence de pellet devant l'IR <=> out = 0V
   absence de pellet devant l'IR <=> out = 3.6V
   @informations:
   une seconde de tapis roulant enclenché engendre +/- 20 intérruptions (10 begin & 10 end)
   @author : Antoine
   @date : 12 août 2019
*/
//-----------------------------------------------------------------------------------------------------------------------------------------
// BIBLIOTEQUE
//-----------------------------------------------------------------------------------------------------------------------------------------
#include <Streaming.h>
#include "Adafruit_MAX31855.h"    // bibliotheque pour la communiction avec le module MAX31855.
#include <SPI.h>                  // bibliotheque complementaire a celle d'adafruit pour la communiction avec le module MAX31855.

//-----------------------------------------------------------------------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------------------------------------------------------------------
// Les Commentaires
//------------------------------
//#define COMMENT               // On/Off commentaire sur moniteur serie /!\ conflit dans la communication serie quand on ouvre l'appli Qt !!!
//#define TIMER                 // Affiche les temps relevés durant le process
//#define FACULTATIF            // implémente des morceaux de codes faculatifs (utile au debogage, pour des informations envoyées en Serial).
//------------------------------
// Les Valeurs des Parametres Variables
//------------------------------
#define spd 70                  // vitesse moteur [ms]
#define prescaler 1             // in fine, defini la resolution du chrono (p/r a sa maj)
#define tempoVerfiBourrage 500  // En moyenne, si la partie mecanique n'a pas de ralentissement involontaire, pour un "spd = 60", il y a environs 15int/sec.
// On va tapper sur un MINIMUM de 3 int/500ms dans ce programme pour la verification du bourrage.
#define tempoDebourrage 1000    //durrée de l'enclenchement du moteur en marche arriere.
#define tempoTempValEmit 1999   // interval de temp auquel est envoyé la température relevée par le thermocouple.
//------------------------------
// L'attribution des broches du µ-controleur
//------------------------------
#define pinInterruptNoDebit 2
#define compteTourInterruptPin 3   //capteur IR sur roue avant du tapis roulant. On verifie si la roue tourne durant la periode motorOn.

#define In1Pin 4
#define In2Pin 5
#define pwmPin 9

// Example creating a thermocouple instance with software SPI on any three digital IO pins :
#define MAXDO   6 //data output
#define MAXCS   7 //chip select
#define MAXCLK  8 //clock

//-----------------------------------------------------------------------------------------------------------------------------------------
// INSTANCIATION OBJET
//-----------------------------------------------------------------------------------------------------------------------------------------
// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

//-----------------------------------------------------------------------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------------------------------------------------------------------
boolean lapsOn = false;
boolean bourrageTapis = false;
volatile boolean caseOne = false;
volatile boolean caseTwo = false;
volatile boolean irState = false;
volatile int interruptionBourrage = 0;
#ifdef FACULTATIF
volatile int pelletInterruptCounter = 0;
#endif
volatile int i = 0; // variabe servant d'index pour remplir les tableaux de relevé de temps des interruptions ("beginning" & "end" PELLET sur le tapis).
int j = 0;
#ifdef TIMER
int t = 0;          //variable servant d'index pour l'affichage des timers si definit.
#endif
int compteBoucle = 0;
int temperature = 0;
int tempsEcoule_DEF = 0;
int dataReceived[8] = {0};
int timeOpenMs = 0;
int timeOpen = 0;
int timeCloseMs = 0;
int timeClose = 0;
char timeOpenChar[4] = {0, 0, 0, '\0'};
char timeCloseChar[4] = {0, 0, 0, '\0'};
double c = 0;
unsigned long currentTime = 0;
unsigned long startTimeClose = 0;
unsigned long tempsEcoule_PRV = 0;
unsigned long lastTimeThermo = 0;
volatile unsigned long chronoPellets = 0;
volatile unsigned long beginningPellets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 25 espaces memoires de 4octets = 100 octets.
volatile unsigned long endPellets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 25 espaces memoires de 4octets = 100 octets.
volatile unsigned long currentInterruptTime = 0;
volatile unsigned long lastInterruptTime = 0;
volatile unsigned long currentInterruptTimeBourrage = 0;
volatile unsigned long lastInterruptTimeBourrage = 0;
volatile unsigned long beginningBourrage = 0;
volatile unsigned long currentTimeVerifBourrage = 0;
volatile unsigned long lastTimeVerifBourrage = 0;


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
  //on lie l'interruption du bourrage tapis a la pin concernée
  attachInterrupt(digitalPinToInterrupt(compteTourInterruptPin), interruptCompteTour, CHANGE);

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
    if (lapsOn) {
      compteBoucle++;
#ifdef COMMENT
      Serial << "Boucle n° : " << compteBoucle << endl;
      Serial << "compteur interruption = " << pelletInterruptCounter << endl;
#endif

      //---------------------------------------------------------------------------------------
      //  GESTION DE la RAZ variable index du tableau de sauvegarde des temps d'interruptions
      //---------------------------------------------------------------------------------------
      if (i == 24) {
        i = 0;
      }
      //---------------------------------------------------------------------------------------
      //  GESTION DE LA MISE A JOUR DU CHRONO PRESENCE PELLET
      //---------------------------------------------------------------------------------------
      if ((compteBoucle >= prescaler) && (beginningPellets[j] != 0)) {
#ifdef COMMENT
        Serial << "MAJ chrono via COMPTE boucle " << endl;
#endif
        caseOne = true;
        UpDateChrono();
        compteBoucle = 0;
#ifdef TIMER
        for (t = 0; t < 10; t++) {
          Serial << " Beginning " << t << " = " << beginningPellets[t] << "\t\t";
          Serial << " End " << t << " = " << endPellets[t] << endl;
        }
        Serial << " Chrono = " << chronoPellets << endl;
#endif
      }
      else if (endPellets[j] != 0) {
#ifdef COMMENT
        Serial << " MAJ chrono via INTERRUPTION FLUX " << endl;
#endif
        caseTwo = true;
        UpDateChrono();
        compteBoucle = 0;
#ifdef TIMER
        for (t = 0; t < 10; t++) {
          Serial << " Beginning " << t << " = " << beginningPellets[t] << "\t\t";
          Serial << " End " << t << " = " << endPellets[t] << endl;
        }
        Serial << " Chrono = " << chronoPellets << endl;
#endif

      }
      currentTimeVerifBourrage = millis();
      if ((currentTimeVerifBourrage - lastTimeVerifBourrage) >= tempoVerfiBourrage) {
        if (interruptionBourrage < 3) {
          motorStop();
          detachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit));
          beginningBourrage = millis();
          endPellets[i] = beginningBourrage; // il faudra p-e voir si parfois, avec l'inertie du moteur, le tapis continue, une interruption se produite et il sauvegarde
          // un millis sur le mauvais endPellets"i" (qui pour le cous n'aura srement pas de beginnning.. )ou mettre un (if beg..)
          bourrageTapis = true;
          lapsOn = false;
          motorBackward();
        }
        else {
          lastTimeVerifBourrage = currentTimeVerifBourrage;
          interruptionBourrage = 0;
        }
      }
      //---------------------------------------------------------------------------------------
      //  GESTION DE L'ARRET DU TAPIS (OVERFLOW CHRONO PRESENCE PELLET)
      //---------------------------------------------------------------------------------------
      if (chronoPellets > timeOpenMs) {
        detachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit));
#ifdef COMMENT
        Serial << "ChronoPellets : OVERFLOW : " << chronoPellets << endl;
#endif
        motorStop();
        lapsOn = false;
        startTimeClose = millis();
        chronoPellets = 0;
        for (int k = 0; k < 25; k++) {
          beginningPellets[k] = 0;
          endPellets[k] = 0;
        }
        j = 0;
        i = 0;
#ifdef FACULTATIF
        pelletInterruptCounter = 0;
#endif
      }
    }
    else if (bourrageTapis) {
      //traitement du bourrage
      currentTime = millis();
      if ((currentTime - beginningBourrage) > tempoDebourrage) {
        motorStop();
        delay(50);
        EIFR = 0x01; // les interrupt ne s'enclenche plus correctement mais flag int RAZ au demarrage..
        attachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit), interruptNoDebit, CHANGE);
        motorForward();
        lastTimeVerifBourrage = millis();
        lapsOn = true;
        bourrageTapis = false;
        interruptionBourrage = 0;
      }
    }
    //---------------------------------------------------------------------------------------
    //  GESTION DU DEMARRAGE DU TAPIS
    //---------------------------------------------------------------------------------------
    else {
      phaseDemarrageTapis();
    }
  }
  //------------------------------------------------------------
  //Envoie des valeurs du thermoCouple sur le port SERIAL
  //------------------------------------------------------------
  //tempsActuel_3 = millis();
  currentTime = millis();
  if ((currentTime - lastTimeThermo) > tempoTempValEmit) {
    //tempsPrec_3 = millis();
    lastTimeThermo = currentTime;
    // basic readout test, print the current temp ambient
    temperature = thermocouple.readInternal();
    // basic readout test, print the current temp on thermocouple
    c = thermocouple.readCelsius();
    // On additionne les deux
    temperature += c;
    //cast et formattage des data's pour l'envoi
    //tempsEcoule_PRV = millis();
    //tempsEcoule_PRV = tempsPrec_3; // je pense que cela devrait prendre moins d'instruction que de recupe le registre timer... p-e pas..
    tempsEcoule_PRV = lastTimeThermo; // je pense que cela devrait prendre moins d'instruction que de recupe le registre timer... p-e pas..
    tempsEcoule_PRV /= 1000;
    tempsEcoule_DEF = (int)tempsEcoule_PRV;
    Serial.print(temperature + String(",") + tempsEcoule_DEF + String("\n"));
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

void phaseDemarrageTapis() {
#ifdef COMMENT
  Serial << "Verif' condition demarrage..." << endl;
#endif
  currentTime = millis();
  if ((currentTime - startTimeClose) > timeCloseMs) {
    irState = digitalRead(pinInterruptNoDebit);
#ifdef COMMENT
    Serial << "Demmarrage tapis :" << endl;
#endif
    if (!irState) {                                   // c a d : presence de pellet devant la cellule
#ifdef COMMENT
      Serial << " ===> AVEC pellet" << endl;
#endif
      beginningPellets[0] = millis();
      //previewsTime = beginningPellets[0];   // ???
      //initializationChrono = true;
    }
    lapsOn = true;
    EIFR = 0x01; // les interrupt ne s'enclenche plus correctement mais flag int RAZ au demarrage..
    attachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit), interruptNoDebit, CHANGE);
    motorForward();
    lastTimeVerifBourrage = millis();
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

void UpDateChrono() {

  //----------------------------------------------------------------------------------------------
  // ((compteBoucle > prescaler) && (beginningPellets[j] != 0)) : GESTION DE LA MAJ DU A UN DEPASSEMENT DE COMPTE BOUCLE
  //----------------------------------------------------------------------------------------------
  if (caseOne) {
    currentTime = millis();
    chronoPellets += (currentTime - beginningPellets[j] ); // ou j-1 ??
    beginningPellets[j] = currentTime;
    caseOne = false;
  }
  //----------------------------------------------------------------------------------------------
  // (endPellets[j] != 0) : GESTION DE LA MAJ DU A UNE INTERRUPTION DU FLUX
  //----------------------------------------------------------------------------------------------
  else if (caseTwo) {
    chronoPellets += (endPellets[j] - beginningPellets[j]);
    if (j == 24) {
      j = 0;
    }
    else {
      j++;
    }
  }
  caseTwo = false;
}



//-----------------------------------------------------------------------------------------------------------------------------------------
// INTERRUPTIONS (Interupt Service Routine)
//-----------------------------------------------------------------------------------------------------------------------------------------

void interruptNoDebit() {
  detachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit));
  irState = digitalRead(pinInterruptNoDebit);
  currentInterruptTime = millis();
  if ((currentInterruptTime - lastInterruptTime) >= 20) {
#ifdef FACULTATIF
    pelletInterruptCounter++;
#endif
    if (irState) {                                        // c a d : absence de pellet devant la cellule
      if (beginningPellets[i] != 0) {
        endPellets[i] = millis();
        i++;
      }
    }
    else {
      beginningPellets[i] = millis();
    }
    lastInterruptTime = currentInterruptTime;
  }
  attachInterrupt(digitalPinToInterrupt(pinInterruptNoDebit), interruptNoDebit, CHANGE);
}

void interruptCompteTour() {
  detachInterrupt(digitalPinToInterrupt(compteTourInterruptPin));
  currentInterruptTimeBourrage = millis();
  if ((currentInterruptTimeBourrage - lastInterruptTimeBourrage) >= 20) {
    interruptionBourrage++;
  }
  lastInterruptTimeBourrage = currentInterruptTimeBourrage;
  attachInterrupt(digitalPinToInterrupt(compteTourInterruptPin), interruptCompteTour, CHANGE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
