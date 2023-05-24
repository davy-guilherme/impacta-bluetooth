// ordem de criacao: dispositivo, servidor, servico, caracteristica
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "DHT.h"

// teremos um servico e uma caracteristica
BLECharacteristic *caracteristicaTX;

#define UUID_SE "dec53176-938a-4de5-9c0d-76024a14b142"
#define UUID_TX "4b7127a2-b827-4bb6-b37d-9e9453554d1c"

#define DHTTYPE DHT11

int pino_dht = 12;

// objeto que controla o sensor
DHT dht(pino_dht, DHTTYPE);

// classe para saber quando uma conexão foi estabelecida 
// importante para que não sejam enviadas informações para "ninguem"
bool dispositivoConectado = false;

class ServerCallbacks: public BLEServerCallbacks {
  void onConnect (BLEServer* servidor_ble) {
    dispositivoConectado = true;
  }
  void onDisconect (BLEServer* servidor_ble) {
    dispositivoConectado = false;
  }
};

void setup() {
  BLEDevice::init("ESP32");

  BLEServer *servidor = BLEDevice::createServer(); // o dispositovo eh quem cria o servidor

  servidor->setCallbacks(new ServerCallbacks()); //instanciando a classe
  
  BLEService *servico = servidor->createService(UUID_SE); // quem cria o servico eh o servidor

  caracteristicaTX = servico->createCharacteristic(UUID_TX, BLECharacteristic::PROPERTY_NOTIFY); // o servico cria a caracteristica

  caracteristicaTX->addDescriptor(new BLE2902());

  servico->start();
  servidor->getAdvertising()->start(); // o servidor comeca a anunciar a si mesmo e suas caracteristicas

}

void loop() {

  if (dispositivoConectado) {
    // leitura da temperatura
    int temp = dht.readTemperature();

    // verifica se eh um valor numerico
    if (!isnan(temp)) { // is not number, verifica se a leitura obteve um valor valido - retorna verdade se o valor for numero não valido e falso se for um numero 

      char info[8];

      dtostrf(temp, 2, 2, info); // transforma inteiro em char

      caracteristicaTX->setValue(info);
      caracteristicaTX->notify();
      
    }
  }


  delay(1000);

}
