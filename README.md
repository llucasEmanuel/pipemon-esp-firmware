# pipemon-esp-firmware

Firmware para um sistema LoRa + ESP32 que envia/recebe leituras de temperatura e vazão e publica via MQTT a partir de um gateway.

Arquivos importantes
- /src/main_gateway.cpp — código do gateway (recebe LoRa, lê sensores locais e publica em MQTT)
- /test/main_tx.cpp — código do transmissor (lê sensores e envia pacotes LoRa)
- /scripts/flow_simulator.py — gera os valores de vazão simulados e escreve em logs/flow.txt
- /scripts/leak_simulator.py — gera os valores de vazão simulados com vazamento e escreve em logs/leak.txt

Principais definições (verificar nos fontes)
- LoRa pins:
  - SCK = 5, MISO = 19, MOSI = 27, SS = 18, RST = 14, DIO0 = 26
  - Banda: 915MHz
- Sensores:
  - Pino do sensor de temperatura: 13
- MQTT:
  - Necessário ajustar as configurações de Wi-Fi de acordo com a sua rede em `src/main_gateway.cpp`

Observação importante para upload/testruns
- Para compilar/flashar o transmissor (main_tx), mova o arquivo do diretório `test` para `src`. Exemplo (Linux):
  - mv test/main_tx.cpp src/main.cpp
- Para testar/manter o gateway sem sobrescrever o build principal, mova o gateway para `test`:
  - mv src/main_gateway.cpp test/main_gateway.cpp

Exemplo de comandos (PlatformIO)
- Build + upload (após mover o arquivo correto para `src`):
  - pio run -t upload

Formato do payload enviado para MQTT
- JSON com campos:
  - device_id
  - temperature_in  (valor recebido via LoRa)
  - temperature_out (sensor local)
  - flow_in         (valor recebido via LoRa)
  - flow_out        (sensor local)
  - timestamp

Exemplo
{
  "device_id":"ESP32Publisher_01",
  "temperature_in":25.4,
  "temperature_out":24.8,
  "flow_in":7.23,
  "flow_out":7.20,
  "timestamp":420
}

Notas finais
- Teste a conectividade LoRa com prints seriais antes de confiar no envio via MQTT.
- Para mudanças rápidas entre os modos TX/GATEWAY, usar os comandos mv acima para trocar qual arquivo está em /src antes de compilar/flashar.
