#include <SPI.h>
#include <LoRa.h>
#include <Temperature.h>
#include <Flow.h>

// Definição dos pinos para o Heltec V2
#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DIO0    26

// Escolha a frequência da sua região (Brasil geralmente usa 915E6)
#define BAND    915E6 

const int tempPin = 13;
TemperatureSensor tempSensor(tempPin);
FlowSensor flowSensor;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciou");

  tempSensor.init();
  flowSensor.init();

  // Inicializa a comunicação SPI nos pinos do Heltec
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Iniciando LoRa...");
  if (!LoRa.begin(BAND)) {
    Serial.println("Erro ao iniciar LoRa. Verifique as conexões!");
    while (1);
  }
  
  LoRa.setTxPower(20); 
  //LoRa.setSpreadingFactor(7);
  Serial.println("LoRa Inicializado com sucesso!");

}


void loop() {
  Serial.print("Enviando pacote: ");
  Serial.println(millis());

  double temp = tempSensor.getTemperature();
  double flow = flowSensor.getFlow();

  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print(" | Vazão: ");
  Serial.println(flow);

  // Inicia o pacote, escreve os dados e finaliza o envio
  LoRa.beginPacket();
  LoRa.print(temp);
  LoRa.print(",");
  LoRa.print(flow);
  LoRa.endPacket();

  delay(3000);
}