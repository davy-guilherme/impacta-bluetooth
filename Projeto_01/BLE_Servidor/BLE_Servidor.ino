#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <PubSubClient.h>
#include <WiFi.h>

//const char* nome_wifi = "2.4G_Apto94";
//const char* senha_wifi = "gatinhos";
const char* nome_wifi = "Quintal";
const char* senha_wifi = "quintal.dev";

//const char* broker = "raspberrypi.local";
const char* broker = "192.168.1.102";

const char* usuario_mqtt = "";
const char* senha_mqtt = "";
const char* cliente_mqtt = "cliente_teste";
const char* topico = "testTopic";
const int porta_mqtt = 1883; // porta padrao para conexao mqtt, apesar de outras tbm poderem ser utilizadas

WiFiClient wifiClient;
PubSubClient client(broker, porta_mqtt, NULL, wifiClient);
// PubSubClient client(espClient);

#define UUID_SER "71096367-0e6a-4b33-bbca-9ef40f71c109"
#define UUID_CAR "e771dfb6-d703-47e2-a866-1df4a46e5a91"

void conectarWifi () {
  Serial.print("Conectando na rede");
  WiFi.begin(nome_wifi, senha_wifi);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a rede");
}

void conectarMQTT () {
  Serial.print("Conectando no Broker");
  client.setServer(broker, porta_mqtt);
  while ( !client.connect(cliente_mqtt, usuario_mqtt, senha_mqtt) ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao Broker");
}

void enviar (String payload) {
  if ( !client.connected() ) {
    conectarMQTT();
  }

  client.publish( topico, (char*) payload.c_str() );
}

  
class CallbacksDeCaracteristica: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *caracteristica) {
    std::string payload = caracteristica->getValue();

    if (payload.length() > 0) {
      enviar(payload.c_str());
    }
    
  }
};


void setup() {
  Serial.begin(115200);

  conectarWifi();
  conectarMQTT();

  BLEDevice::init("ESP32");

  BLEServer *servidor = BLEDevice::createServer();

  BLEService *servico = servidor->createService(UUID_SER);

  BLECharacteristic *caracteristica = servico->createCharacteristic(UUID_CAR, BLECharacteristic::PROPERTY_WRITE); //permite que dispositivos conectados a esta caracteristica escrevam valores para ela

  caracteristica->setCallbacks( new CallbacksDeCaracteristica() );

  servico->start();

  servidor->getAdvertising()->addServiceUUID(UUID_SER);

  servidor->getAdvertising()->start();

}

void loop() {
  
  delay(1000);

}
