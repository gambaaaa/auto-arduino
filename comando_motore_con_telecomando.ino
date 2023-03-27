#include <ESP32Servo.h>
#include <IRremote.hpp>

#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Rightx          GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)
#define IR_PIN 7

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

IRrecv receiver(IR_PIN); // ricevitore
decode_results results;

unsigned long key_value = 0; // tasto che premo --> per salvare il vecchio stato / tasto premuto

bool isOn = false;

int speedCar = 800;         // 400 - 1023.
int speed_Coeff = 3;

void setup() {
 
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT); 
  
  Serial.begin(115200);  
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

void loop() {
 
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

  delay(50);
}
