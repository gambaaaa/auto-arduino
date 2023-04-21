#include <ESP32Servo.h>
#include <IRremote.hpp>


#define ENA   23          // Enable/speed motors Right        GPIO14(D5)
#define ENB   22          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  19          // L298N in1 motors Rightx          GPIO15(D8)
#define IN_2  18          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  4           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  5           // L298N in4 motors Left            GPIO0(D3)
/*
  PROTOCOLLO IR USATO : NEC
*/
#define RECV_PIN 7         // Pin ricevitore IR  - IR_PIN
#define TRIG 15
#define ECHO 2
#define BUZZER_PASSIVO 13
#define LED_VERDE 32
#define LED_RGB_R 27
#define LED_RGB_G 14
#define LED_RGB_B 12
#define LED_IR_ON 35     // Eventualmente cambiare 
#define LEDC_CHANNEL_R 0 // Il canale 0 è del rosso
#define LEDC_CHANNEL_G 1 // Il canale 1 è del verde
#define LEDC_CHANNEL_B 2 // Il canale 2 è del blu
#define LEDC_TIMER_8_BIT 8 // PWM a 8 bit (codice RGB a 8 bit)
#define LEDC_BASE_FREQ 10000

/*
  Strutture dati per gestione ricezione Comando IR
*/

// Dichiarazione delle funzioni inerenti la gestione IR precedente alla struct vectoredCode

void goAhead();
void goBack();
void goRight();
void goLeft();
void goAheadRight();
void goAheadLeft();
void goBackRight();
void goBackLeft();
void powerOn();
void stopCar();


// Definizione di un nuovo tipo di dato per definire un puntatore a funzione da usare come valore di ritorno di una funzione. (per ovviare problemi di compilazione)
typedef void (*ptr)();
typedef ptr (*pt)();

/*
  "Codici Vettorizzati": per codici si intende il codice associato allo specifico segnale IR ricevuto

  TASTI UTILIZZATI per gestire i movimenti della macchina : 

  1 -> AVANTI - SX
  2 -> AVANTI  
  3 -> AVANTI - DX
  4 -> SX
  5 -> NON USATO OPPURE IMPLEMENTABILE COME STOP
  6 -> DX
  7 -> INDIETRO - SX
  8 -> INDIETRO
  9 -> INDIETRO - DX
*/

struct vectoredCode {

  uint32_t codeValue;  // Codice IR
  void (*action)();    // Funzione da eseguire associata allo specifico codice
  const char *actionDescription;   // Breve descrizione / titolo della funzione associata al codice: itile per eventuale debug, logfile...

} codeList [] = {

  {0xBA45FF00, &powerOn, "power"} , // La macchina si accende --> POWER
  {0xB847FF00, &stopCar, "stop"} , // La macchina si ferma --> STOP 
  {0xE718FF00, &goAhead, "goAhead"} , // La macchina va avanti --> 2
  {0xAD52FF00, &goBack, "goBack"} , // La macchina torna indietro --> 8
  {0xA55AFF00, &goRight, "goRight"} , // La macchina va a dx --> 6
  {0xF708FF00, &goLeft, "goLeft"} , // La macchina va a sx --> 4
  {0xA15EFF00, &goAheadRight, "goAheadRight"}, // La macchina a va avanti verso dx --> 3
  {0xF30CFF00, &goAheadLeft, "goAheadLeft"}, // La macchina a va avanti verso sx --> 1
  {0xB54AFF00, &goBackRight, "goBackRight"}, // La macchina torna indietro verso dx --> 9
  {0xBD42FF00, &goBackLeft,  "goBackLeft"}, // La macchina torna indietro verso sx --> 7
  
};

// Funzione che restituisce la descrizione dell'azione/funzione inerente uno specifico codice
const char * getActionDescription(uint32_t codeValue) { // Da usare con uno switch value

  size_t codeActionCount = sizeof codeList / sizeof codeList[0];
  for (size_t i = 0; i < codeActionCount; i++)
  {
    if (codeList[i].codeValue == codeValue)
    {
      return codeList[i].actionDescription;
    }
  }
  return ""; // Significa che non è stata ritrovata nessuna associazione
}


// Funzione che restituisce il puntatore alla funzione associata ad uno specifico codice
ptr getFunctionAction(uint32_t codeValue) { // Da usare con uno switch value

  size_t codeActionCount = sizeof codeList / sizeof codeList[0];
  for (size_t i = 0; i < codeActionCount; i++)
  {
    if (codeList[i].codeValue == codeValue)
    {
      return codeList[i].action;
    }
  }
  return &nothing; // Significa che non è stata ritrovata nessuna associazione - vedi funzione nothing
}

IRrecv receiver(RECV_PIN); // Ricevitore: vedi IRremote.hpp
decode_results results;
// void (*p)();
ptr p; // Puntatore che "riferisce" una funzione associata ad un eventuale codice che viene gestito (codice IR)

unsigned long key_value = 0; // Tasto che premo --> per salvare il vecchio stato / tasto premuto
long readout;

float distanza;

bool isOn = false; // Stato macchina: isOn = false ==> macchina spenta | isOn = true ==> ho premuto Power 

int tono;
int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

//----------------------

void setup() {
    
  Serial.begin(19200); 
  Serial.println("Ciao!");
  
  ledcSetup(LEDC_CHANNEL_R, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // Inizializza i canali
  ledcAttachPin(LED_RGB_R, LEDC_CHANNEL_R); // Assegna i pin ai canali
  ledcSetup(LEDC_CHANNEL_G, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // Inizializza i canali
  ledcAttachPin(LED_RGB_G, LEDC_CHANNEL_G); // Assegna i pin ai canali
  ledcSetup(LEDC_CHANNEL_B, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // Inizializza i canali
  ledcAttachPin(LED_RGB_B, LEDC_CHANNEL_B); // Assegna i pin ai canali
  receiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  
  pinMode(LED_RGB_R, OUTPUT);
  pinMode(LED_RGB_G, OUTPUT);
  pinMode(LED_RGB_B, OUTPUT); 
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT); 
  pinMode (TRIG, OUTPUT);
  pinMode (ECHO, INPUT);
  pinMode (LED_VERDE, OUTPUT);
  pinMode(LED_IR_ON, OUTPUT);
  welcomeLed(); 
}

void suonaBuzzerDistanza(float distanza) {

    /* 
      colore:   R     G     B
      rosso:    255   0     0
      verde:    0     255   0
      arancio:  255   128   0
    */

  if ((int) distanza <= 2) {
    RGB_Color(255, 0, 0);
    delay(250);
    RGB_Color(0, 0, 0);
    beepBuzzer(BUZZER_PASSIVO, 2000, 100);
  } else if ((int) distanza <= 4 && (int) distanza > 2) {
    RGB_Color(255, 0, 0);
    beepBuzzer(BUZZER_PASSIVO, 1600, 250);
  } else if ((int) distanza <= 6 && (int) distanza > 4) {
    RGB_Color(255, 128, 0);
    beepBuzzer(BUZZER_PASSIVO, 1200, 500);
  } else if ((int) distanza <= 8 && (int) distanza > 6) {
    RGB_Color(255, 128, 0);
    beepBuzzer(BUZZER_PASSIVO, 800, 750);
  } else if ((int) distanza <= 10 && (int) distanza > 8) {
    RGB_Color(255, 128, 0);
    beepBuzzer(BUZZER_PASSIVO, 400, 1000);
  } else {
    RGB_Color(0, 255, 0); 
    noTone(BUZZER_PASSIVO);
  }
}

void beepBuzzer(int pin, int freq, int delayTime) {
  // delayTime in ms, freq in Hz
  tone(pin, freq);
  delay(delayTime);
  noTone(pin);
  delay(delayTime);
}

void RGB_Color(int R, int G, int B) {
  R = 255 - R;
  G = 255 - G;
  B = 255 - B;
  analogWrite(LED_RGB_R, R);
  analogWrite(LED_RGB_G, G);
  analogWrite(LED_RGB_B, B);  
}

void goAhead() { 

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goBack() { 

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goRight() { 

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goLeft() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goAheadRight() {
      
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar/speed_Coeff);
 
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goAheadLeft() {
      
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar/speed_Coeff);
}

void goBackRight() { 

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar/speed_Coeff);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goBackLeft(){ 

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar/speed_Coeff);
}

void powerOn() { // Abilito il movimento e accendo la macchina

  digitalWrite(LED_IR_ON, HIGH);
  delay(200);  
  digitalWrite(LED_IR_ON, LOW);
  delay(200); 
  digitalWrite(LED_IR_ON, HIGH);
  delay(200);  
  digitalWrite(LED_IR_ON, LOW);
  delay(200); 

  digitalWrite(LED_IR_ON, HIGH);

  isOn = true;
}

void stopCar() {  // PowerOff

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);

      digitalWrite(LED_IR_ON, LOW);

      isOn = false;
}

void nothing() {

  Serial.println("Codice non riconosciuto / non previsto");
}

void welcomeLed() {
  
  digitalWrite(LED_VERDE, HIGH);
  delay(500);  
  digitalWrite(LED_VERDE, LOW);
  delay(500); 
  digitalWrite(LED_VERDE, HIGH);
  delay(500);  
  digitalWrite(LED_VERDE, LOW);
  delay(500); 
}

void loop() {
 
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(9);  //impulso da 10us
  digitalWrite(TRIG, LOW);
  readout = pulseIn(ECHO, HIGH);
  distanza = (float) readout/58; //velocità A/R = 340m/s => distanza(cm) = T(ms)*17 = T(us)*0,017 = T(us)/58 
  Serial.println(distanza);
  suonaBuzzerDistanza(distanza);

  //Gestione IR

  /*
     * Controllo se ci sono dati disponibili/ se son stati ricevuti dei dati e se sì, prova a decodificarli.
     * Il risultato decodificato è nella struttura IrReceiver.decodedIRData.
     *
     * Direttamente da IRremote.hpp: 
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
  */

  if (receiver.decode()) {
    // Stampa utile per debug e check funzionamento della gestione IR
    if(receiver.decodedIRData.decodedRawData == 0) { // CASO RIPETIZIONE TASTO (associato ad uno specifico codice)- SIGNIFICATIVO  
      Serial.println(key_value, HEX); // In key_value mantengo / memorizzo il codice associato alla pressione di tasti(del trasmettitore IR) differenti tra loro
    } else {
      Serial.println(receiver.decodedIRData.decodedRawData, HEX); // Stampa del relativo codice ricevuto
      
      key_value = (receiver.decodedIRData.decodedRawData);
      p = getFunctionAction(key_value);
      const char *actionDescr = getActionDescription(key_value);
      
      Serial.println(actionDescr);
      
      if(actionDescr!="") { // Se il codice è riconosciuto tra quelli previsti in codeList - struct
          if(!isOn && actionDescr=="power") { // Solo in corrispondenza di power a macchina spenta, viene eseguita la funzione riferita da p
             (*p)(); //p punta a powerOn()
          } else if(isOn && actionDescr!="power") { // Solo se la macchina è accesa posso "utilizzarla"
              (*p)(); // Sicuramente p non punta a powerOn poichè è già accesa 
          }
      } else { // Codice non è riconosciuto - la macchina rimane ferma
        (*p)(); // p riferisce la funzione nothing --> Serial.println("Codice non riconosciuto");
      }
    }
  /*
    !!!IMPORTANTE!!!
      -->Abilito la ricezione del valore successivo,
         poiché la ricezione si è interrotta dopo la fine del pacchetto di dati attualmente ricevuto.(dell'attuale codice 0x....)
  */
    receiver.resume(); // Enable receiving of the next value
  }

  delay(100);
}
