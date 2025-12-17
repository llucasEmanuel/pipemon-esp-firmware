#include <Arduino.h>
#include <WiFi.h> // Para ESP32
//#include <ESP8266WiFi.h> // Se estiver usando ESP8266
#include <PubSubClient.h>
#include <ArduinoJson.h> // Inclua a nova biblioteca
#include <Temperature.h>
#include <LoRa.h>

// --- Configurações de Rede ---
const char* ssid = "ROBOCIN_CORE";
const char* password = "robocincore2024";

// --- Configurações do Broker Mosquitto ---
const char* mqtt_server = "192.168.1.171"; // Ex: "192.168.1.100" ou "test.mosquitto.org"
const int mqtt_port = 1883;

// --- Configurações MQTT do Dispositivo ---
const char* clientID = "ArduinoPublisher_01";
const char* publishTopic = "gateway/data";

// Variáveis para controle de tempo
long lastMsg = 0;
int value = 0;

const int tempPin = 13;
TemperatureSensor tempSensor(tempPin);

// Inicialização dos Clientes
WiFiClient espClient;
PubSubClient client(espClient);

// --- 1. Funções de Conexão Wi-Fi ---

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

// --- 2. Função de Reconexão MQTT ---

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

// --- SETUP Principal ---
void setup() {
  Serial.begin(115200);
  
  tempSensor.init();
  
  setup_wifi();
  
  // Configurações do servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  // Não precisamos da função de callback 'client.setCallback()', pois este é apenas um PUBLISHER.
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    value++; // Incremento para simular mudança nos dados

    // 1. Criar o documento JSON (192 bytes é seguro para esses campos)
    StaticJsonDocument<192> doc;
    
    float temp = tempSensor.getTemperature();

    if (!tempSensor.isConnected()) {
      Serial.println("Sensor de temperatura não conectado");
    }

    // 2. Preencher com os campos solicitados
    doc["device_id"] = clientID;
    doc["temperature_in"] = temp;  // Simulado
    doc["temperature_out"] = 22.1 + (float)value / 12.0; // Simulado
    doc["flow_in"] = 1.5 + (float)value / 5.0;           // Simulado
    doc["flow_out"] = 1.4 + (float)value / 5.0;          // Simulado
    
    // Para o timestamp, como o ESP32 não tem relógio de tempo real (RTC) nativo sincronizado,
    // o ideal é o seu backend/Python colocar o tempo ao receber. 
    // Mas se quiser enviar o tempo de atividade (uptime) do chip:
    doc["timestamp"] = millis() / 1000; 

    // 3. Serializar para buffer
    char buffer[192];
    serializeJson(doc, buffer);

    // 4. Publicar no tópico
    Serial.print("Enviando JSON para MQTT: ");
    Serial.println(buffer);
    client.publish(publishTopic, buffer);
  }
}