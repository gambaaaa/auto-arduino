// autore: Gabriele Festa
#include <ESP32Servo.h>
#include <IRremote.hpp>

#define TRIG 21
#define ECHO 22
#define BUZZER_PASSIVO 25
#define LED_ROSSO 12
// #define LED_GIALLO 14
#define LED_BLU 14
#define LED_VERDE 27
#define IR_PIN 7

long readout;
float distanza;
int tono;
unsigned long key_value = 0; // tasto che premo --> per salvare il vecchio stato / tasto premuto
bool isOn = false;

IRrecv receiver(IR_PIN); // ricevitore
decode_results results;

void setup() {
  Serial.begin(115200);
  receiver.enableIRIn(); // start receiver 
  pinMode (TRIG, OUTPUT);
  pinMode (ECHO, INPUT);
  pinMode (LED_ROSSO, OUTPUT);
  pinMode (LED_BLU, OUTPUT);
  pinMode (LED_VERDE, OUTPUT);
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
  digitalWrite(LED_ROSSO, R);
  digitalWrite(LED_VERDE, G);
  digitalWrite(LED_BLU, B);
}

void receiveCommand(unsigned long value) {
  switch(value) {
      case 0xFFA25D: 
        Serial.println("-->Hai premuto il pulsante power");
        break;
      /*
      case 0xFF6897: 
        Serial.println("--> Hai premuto 0 (base DEC)");
        break;
      */
      case 0xFF18E7: 
        Serial.println("--> Hai premuto 2 (la macchina andrebbe avanti)");
        // RGB_color(0, 255, 0);
        // TO-DO: muovi motore
        break;
      case 0xFF10EF: 
        Serial.println("--> Hai premuto 4 (la macchina andrebbe a sx)");
        // RGB_color(0, 255, 0);
        // TO-DO: muovi motore
        break;
      case 0xFF4AB5: 
        Serial.println("--> Hai premuto 8 (la macchina andrebbe indietro)");
        // RGB_color(0, 255, 0);
        // TO-DO: muovi motore
        break;
      case 0xFF5AA5: 
        Serial.println("--> Hai premuto 6 (la macchina andrebbe a dx)");
        // RGB_color(0, 255, 0);
        // TO-DO: muovi motore
        break;
      case 0xFFE21D: 
        Serial.println("--> Hai premuto STOP(la macchina si arresterebbe)");
        /* 
          isOn = false;
          RGB_color(255, 0, 0);
          delay(500);
          RGB_color(0, 0, 0);
          delay(500);
          RGB_color(255, 0, 0);
          delay(500);
          RGB_color(0, 0, 0);
          delay(500);
          RGB_color(255, 0, 0);
          delay(500);
          RGB_color(0, 0, 0);
          delay(500);
          // TO-DO: bomb();
        */
        break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(9);  //impulso da 10us
  digitalWrite(TRIG, LOW);
  readout = pulseIn(ECHO, HIGH);
  distanza = (float) readout/58; //velocità A/R = 340m/s => distanza(cm) = T(ms)*17 = T(us)*0,017 = T(us)/58 
  suonaBuzzerDistanza(distanza);

  /*
  while(!isOn) {
    if(results.value == 0xFFA25D) {
      isOn = true;
    }
  }

  if (receiver.decode(&results)) {
    if (results.value == 0XFFFFFFFF) {
       // ottengo il codice 0XFFFFFFFF se premo un pulsante a lungo --> dunque il pulsante  che premo(results.value) corrisponde al tasto 
       // schiacciato in precedenza memorizzato in keyvalue
      results.value = key_value;
    }
    receiveCommand(results.value);    
    key_value = results.value;//qui ho ancora il valore attuale --> precedente al resume
    receiver.resume();//per ricevere nuovo valore
  } else {
    RGB_Color(0, 255, 0); 
  }
  */

  delay(100);
}
