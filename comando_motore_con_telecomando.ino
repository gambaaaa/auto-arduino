//#include <IRremote.hpp>

#define ENA   23          // Enable/speed motors Right        GPIO14(D5)
#define ENB   22          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  19          // L298N in1 motors Rightx          GPIO15(D8)
#define IN_2  18          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  4          // L298N in3 motors Left            GPIO2(D4)
#define IN_4  5           // L298N in4 motors Left            GPIO0(D3)
#define IR_PIN 7
#define TRIG 15
#define ECHO 2
#define BUZZER_PASSIVO 13
#define LED_VERDE 32
#define LED_RGB_R 27
#define LED_RGB_G 14
#define LED_RGB_B 12
#define LEDC_CHANNEL_R 0 // il canale 0 è del rosso
#define LEDC_CHANNEL_G 1 // il canale 1 è del verde
#define LEDC_CHANNEL_B 2 // il canale 2 è del blu
#define LEDC_TIMER_8_BIT 8 // PWM a 8 bit (codice RGB a 8 bit)
#define LEDC_BASE_FREQ 10000

//IRrecv receiver(IR_PIN); // ricevitore
//decode_results results;

unsigned long key_value = 0; // tasto che premo --> per salvare il vecchio stato / tasto premuto
long readout;

float distanza;

bool isOn = false;

int tono;
int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

void setup() {
    
  Serial.begin(19200); 
  Serial.println("Ciao!");
  
  ledcSetup(LEDC_CHANNEL_R, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // inizializza i canali
  ledcAttachPin(LED_RGB_R, LEDC_CHANNEL_R); // Assegna i pin ai canali
  ledcSetup(LEDC_CHANNEL_G, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // inizializza i canali
  ledcAttachPin(LED_RGB_G, LEDC_CHANNEL_G); // Assegna i pin ai canali
  ledcSetup(LEDC_CHANNEL_B, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT); // inizializza i canali
  ledcAttachPin(LED_RGB_B, LEDC_CHANNEL_B); // Assegna i pin ai canali
  //receiver.enableIRIn();
  
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

void goAhead(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
}

void goBack(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void goRight(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void goLeft(){

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void goAheadRight(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar/speed_Coeff);
 
      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
   }

void goAheadLeft(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar/speed_Coeff);
  }

void goBackRight(){ 

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

void stopCar(){  

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
}

void moveCar(unsigned long value) {
  switch(value) {
      // TO-DO: Aggiungere caso avanti-sx / avanti-dx / dietro-sx / dietro-dx 
      /*
      case 0xFF6897: 
        Serial.println("--> Hai premuto 0 (base DEC)");
        break;
      */
      case 0xFF18E7: 
        Serial.println("--> Hai premuto 2 (la macchina va avanti)");
        goAhead();
        break;
      case 0xFF10EF: 
        Serial.println("--> Hai premuto 4 (la macchina va a sx)");
        goLeft();
        // RGB_color(0, 255, 0);
        break;
      case 0xFF4AB5: 
        Serial.println("--> Hai premuto 8 (la macchina va indietro)");
        goBack();
        // RGB_color(0, 255, 0);
        break;
      case 0xFF5AA5: 
        Serial.println("--> Hai premuto 6 (la macchina andrebbe a dx)");
        goRight();
        // RGB_color(0, 255, 0);
        break;
      case 0xFFE21D: 
        Serial.println("--> Hai premuto STOP (la macchina si arresterebbe)");
        isOn = false;
        stopCar();
        break;
    }
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

  /*
  if (receiver.decode(&results)) {
    if(results.value == 0xFFA25D) {
      isOn = true;
    }
   
    if (results.value == 0XFFFFFFFF) {
       // ottengo il codice 0XFFFFFFFF se premo un pulsante a lungo --> dunque il pulsante  che premo(results.value) corrisponde al tasto 
       // schiacciato in precedenza memorizzato in keyvalue
      results.value = key_value;
    }
    
    if (isOn) {
      moveCar(results.value);    
      key_value = results.value;//qui ho ancora il valore attuale --> precedente al resume
      receiver.resume();//per ricevere nuovo valore
    }
  } else {
    digitalWrite(LED_VERDE, LOW);
  }
  */

  delay(100);
}
