#include "BluetoothSerial.h";
#include <ESP32Servo.h>

BluetoothSerial ESP_BT;
Servo servo;

int pinoServo = 12;

void setup() {
  Serial.begin(115200);
  ESP_BT.begin("ESP32");

  Serial.println("O dispositivo bluetooth esta pronto para ser pareado");

  // servo.setPeriodHertz(50);
  // servo.attach(pinoServo, 1000, 2000);
  
}

void loop() {
  
  if ( ESP_BT.available() ) {
    int comando = ESP_BT.read(); //ASCII

    Serial.print("Recebido: "); Serial.println(comando);

    if ( comando == 49 ) { // 1
      // servo.write(180);
    }

    if ( comando == 48 ) { // 0
      // servo.write(0);
    }

    delay(20);
  }

}
