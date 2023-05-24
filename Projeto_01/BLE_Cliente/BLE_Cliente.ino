#include "BLEDevice.h";

static BLEUUID UUID_SER("71096367-0e6a-4b33-bbca-9ef40f71c109"); //servico
static BLEUUID UUID_CAR("e771dfb6-d703-47e2-a866-1df4a46e5a91"); //caracteristica

// variaveis de conrole - verifica se tem conexao ativa e se o servidor foi conectado
static boolean encontrado = false;
static boolean conectado = false ;

static BLERemoteCharacteristic* caracteristica;
static BLEAdvertisedDevice* servidor;

const int pinoLDR = A5;

class CallbacksDeConexao: public BLEClientCallbacks {
  
  void onConnect (BLEClient *cliente) {
    conectado = true;
    Serial.println("Conectado.");
  }

  void onDisconnect (BLEClient* cliente) {
    conectado = false;
    Serial.println("Desconectado.");
  }
  
};

class CallbackDeBusca: public BLEAdvertisedDeviceCallbacks {

  void onResult (BLEAdvertisedDevice dispositivo) {
    if ( dispositivo.haveServiceUUID() && dispositivo.isAdvertisingService(UUID_SER) ) {
      BLEDevice::getScan()->stop();

      servidor = new BLEAdvertisedDevice(dispositivo);

      encontrado = true;
    }
  }
  
};

void conectar () {
  BLEClient* cliente = BLEDevice::createClient();

  cliente->setClientCallbacks( new CallbacksDeConexao() );

  cliente->connect( servidor );

  BLERemoteService* servico = cliente->getService( UUID_SER );

  caracteristica = servico->getCharacteristic(UUID_CAR);
}

void setup() {
  Serial.begin(115200);
  pinMode(pinoLDR, INPUT);

  BLEDevice::init(""); // não precisa de nome pois ete dispositivo não sera buscado, ele é apenas um cliente

  BLEScan* scanner = BLEDevice::getScan();

  scanner->setAdvertisedDeviceCallbacks( new CallbackDeBusca () );
  scanner->setInterval(1349); // controlam alguns processor de busca do scanner
  scanner->setWindow(449); // podem ser valores sempre fixos
  scanner->setActiveScan(true);
  scanner->start(30, false); // por quanto tempo o processo de scanner vai ser executado ate que desista de encontrar o dispositivo EM SEGUNDOS. o callback de busca tbm é capaz de encerrar o processo
  
}

void loop() {
  // so pode enviar o valor se: o dispositivo que queremos nos conectar for encontrado e a conexao foi bem sucedida
  if (encontrado) {
    if (conectado) {
      float valor = analogRead(pinoLDR);
      String nome = "luminosidade";

      String payload = "{\"d\":{\"" + nome + "\": " + valor + "}}"; //convencionado para trechos de informacoes em servicos na internet // {d: {"luminosidade": valor}}

      caracteristica->writeValue(payload.c_str(), payload.length()); // c_str forca a ser uma string caso algo tenha ocorrido no processo - o segundo parametro eh o comprimento da mensagem em caracteres
      Serial.println( valor );
      
    } else {
      conectar();
      Serial.println("Nao conectado");
    }
    
  } else {
    BLEDevice::getScan()->start(5);
    Serial.println("nao encontrado");
  }

  delay(2000);

}
