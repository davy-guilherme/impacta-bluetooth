#include <BLEDevice.h> // permite utilizar low energy
#include <BLEServer.h> // configura como servidor, não como cliente
#include <BLEUtils.h> // componentes para controlar conexão
#include <BLE2902.h>

#define UUID_SE "bf592f34-45ee-410b-b5fa-96e131c93d86" // ID do serviço que será exposto
#define UUID_RX "80b10197-bcad-4c80-96b8-d1a1c0a3e80b" // ID da característica do serviço

const int LED = 12; // pino onde o LED esta conectado

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite (BLECharacteristic *caracteristica) {
    std::string valor = caracteristica->getValue();

    if (valor.length() > 0 ) {
      if ( valor.find("L1") != -1 ) { // se encontrar L1
        digitalWrite(LED, HIGH); // acender o LED
      } else if (valor.find("L0") != -1) {
        digitalWrite(LED, LOW);
      }
    }
  }
};

void setup() {
  pinMode(LED, OUTPUT);

  BLEDevice::init("ESP32");

  BLEServer *servidor = BLEDevice::createServer();

  BLEService *servico = servidor->createService(UUID_SE);

  BLECharacteristic *caracteristicaRX = servico->createCharacteristic(UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  caracteristicaRX->setCallbacks( new CharacteristicCallbacks() );

  servico->start();
  servidor->getAdvertising()->start();

}

void loop() {
  // put your main code here, to run repeatedly:

}
