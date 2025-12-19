#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Temperature.h>
#include <LoRa.h>
#include <SPI.h>
#include <Flow.h>

// Pinos de configuração do LoRa
#define SCK   5
#define MISO  19
#define MOSI  27
#define SS    18
#define RST   14
#define DIO0  26

#define BAND 915E6  

const char* ssid = "ROBOCIN_CORE";
const char* password = "robocincore2024";

const char* mqtt_server = "192.168.1.171";
const int mqtt_port = 1883;

const char* clientID = "ESP32Publisher_01";
const char* publishTopic = "gateway/data";

long lastMsg = 0;

const int tempPin = 13;
TemperatureSensor tempSensor(tempPin);

FlowSensor flowSensor;

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Reconexão do MQTT
void reconnect() {
  // Loop até que o cliente esteja conectado
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Tenta se conectar
    if (client.connect(clientID)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      // Espera 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  tempSensor.init();
  flowSensor.init();
  
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  // Espera a chegada do pacote LoRa
  if (!LoRa.begin(BAND)) {
    Serial.println("Erro ao inicializar LoRa");
    while(1);
  }
  LoRa.setSpreadingFactor(7);

  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 30000) {

    double tempIn = -1;
    double flowIn = -1;

    // Verificação de pacotes do LoRa
    int packetSize = 0;
    do {
      packetSize = LoRa.parsePacket();
    } while (!packetSize);
    if (packetSize) {
      String data = "";
      while (LoRa.available()) {
        data += (char)LoRa.read();
      }

      int commaInd = data.indexOf(',');
      if (commaInd != -1) {
        String tempStr = data.substring(0, commaInd);
        String flowStr = data.substring(commaInd+1);

        tempIn = tempStr.toDouble();
        flowIn = flowStr.toDouble();

        Serial.print("Recebido por LoRa => ");
        Serial.print("Temperatura de Entrada: "); Serial.print(tempIn);
        Serial.print(" | Vazão de Entrada: "); Serial.println(flowIn);
      }
    }

    lastMsg = now;

    StaticJsonDocument<192> doc;
    
    float tempOut = tempSensor.getTemperature();

    if (!tempSensor.isConnected()) {
      Serial.println("Sensor de temperatura não conectado");
    }

    double flowOut = flowSensor.getFlow();

    doc["device_id"] = clientID;
    doc["temperature_in"] = tempIn;
    doc["temperature_out"] = tempOut; 
    doc["flow_in"] = flowIn;           
    doc["flow_out"] = flowOut;
    
    doc["timestamp"] = millis() / 1000; 

    // Serializar o JSON antes do enviar
    char buffer[192];
    serializeJson(doc, buffer);

    Serial.print("Enviando JSON para MQTT: ");
    Serial.println(buffer);
    client.publish(publishTopic, buffer);
  }
}