#include <PCF8574_ESP.h>
#include <PCF8583.h>
#include <ESP8266WiFi.h> // Inclusão de biblioteca do ESP8266 WiFi
#include <PubSubClient.h> // Inclusão da biblioteca de Publicador/Subscrito
#define MSG_BUFFER_SIZE (50) // Tamanho do buffer de recebimento
char msg[MSG_BUFFER_SIZE];                                                                        // A Mensagem será do tipo char e do tamanho definido pelo buffer de recebimento // 
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;  
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 

DallasTemperature sensors(&oneWire);
//const char* ssid = "COW";                                                          // ID de conexão 
//const char* password = "juliobola"; 

const char* ssid = "Autoitec2";                                                          // ID de conexão 
const char* password = "12@utoitec34"; 

// senha de conexão
//const char* mqtt_server = "152,67,42,56";
const char* mqtt_server = "broker.mqtt-dashboard.com";                  //Broker da comunicação MQTT

float temperatura;
float tensao;
byte contador = 0;
// endereço do broker
WiFiClient espClient;                                                                             // Cliente ESP WiFi
PubSubClient client(espClient);                                                                   // Cliente ESP Publicador/Subscrito
unsigned long lastMsg = 0;                                                                        // define última mensagem com valor inicial igual a zero
int value = 0;                                                                                    // variável de valor inteiro inicializada em zero

char Sala;                                                                                        // inclui variável char de nome Sala /LED D2/
char Quarto;                                                                                      // inclui variável char de nome Quarto /LED D4/
char Cozinha;                                                                                     // inclui variável char de nome Cozinha /LED D5/ 
char Banheiro;                                                                                    // inclui variável char de nome Banheiro /LED D7/

void setup() {                                                                                    // laço de configuração 
  /////////Sensor de Temperatura//////////                                                           
  sensors.begin();
  sensors.requestTemperatures(); 
  temperatura = sensors.getTempCByIndex(0);                                                              // Printa "Sensor DHT22"
  //sensor_t sensor;                          
  Serial.println(F("------------------------------------"));                                      // Printa quebra de linha com ----
  Serial.println(F("Temperatura"));                                                               // Printa "Temperatura"
  Serial.print  (F("Sensor: ")); Serial.println(temperatura);                                     // Printa o nome do sensor
  Serial.println(F("------------------------------------"));                                      // quebra de linha com ----
 
  /////////Sensor de Umidade//////////
  int Analogica_0 = analogRead(A0);   // Ler o pino Analógico A0 onde está o LDR
  tensao = Analogica_0 * (3.3 / 1024.0);
  Serial.println(F("------------------------------------"));                                     // Printa quebra de linha com ----
  Serial.println(F("Tensao"));                                                                   // Printa "Umidade"
  Serial.print  (F("Sensor: ")); Serial.println(tensao);                                     // Printa o nome do sensor
  Serial.println(F("------------------------------------"));                                      // quebra de linha com ----
   
  Sala = 5;   //era 4                                                                                    // define Sala como o LED ligado ao pin 4 (D2 do ESP8266)
  Quarto = 2;                                                                                     // define Quarto como o LED ligado ao pin 2 (D4 do ESP8266)
  Cozinha = 14;                                                                                   // define Cozinha como o LED ligado ao pin 14 (D5 do ESP8266)
  Banheiro = 13;                                                                                  // define Banheiro como o LED ligado ao pin 13 (D7 do ESP8266)

  pinMode(Sala, OUTPUT);                                                                          // configura Sala como saída
  pinMode(Quarto, OUTPUT);                                                                        // configura Quarto como saída
  pinMode(Cozinha, OUTPUT);                                                                       // configura Cozinha como saída
  pinMode(Banheiro, OUTPUT);                                                                      // configura Banheiro como saída
  
  Serial.begin(115200);                                                                           // define baudrate da comunicação serial
  
  setup_wifi();                                                                                   // configuração WiFi
  //client.setServer("broker.hivemq.com" , 1883);                                                            // seta servidor como mqtt de port 1883
  client.setServer("152.67.42.56" , 1883);
  client.setCallback(func);
  //client.setCallback(callback);                                                                   // seta mensagem de retorno
}
void func(char* topic, byte* payload, unsigned int length){Serial.println("J");}
void setup_wifi() {                                                                               // inicia laço de configuração WiFi

  delay(10);                                                                                      // atraso de 0,01S;
  Serial.println("");                                                                             // quebra de linha
  Serial.print("Conectando com ");                                                                // printa na serial "Conectado com"
  Serial.println(ssid);                                                                           // printa ID da rede

  WiFi.begin(ssid, password);                                                                     // inicializa o WiFi com o uso da ID de rede e da senha

  while (WiFi.status() != WL_CONNECTED) {                                                         // enquanto o WiFi não estiver conectado, printa na serial "."
    delay(500);                                                                                   // atraso de 0.5 para aparecimento de um novo "."
    Serial.print(".");                                    
  }                  

  Serial.println("");                                                                             // quebra de linha 
  Serial.println("WiFi conectado");                                                               // printa "WiFi conectado"
  Serial.println("IP: ");                                                                         // printa "IP:"
  Serial.println(WiFi.localIP());                                                                 // printa na serial o IP da conexão local
}

void recebe_comandos(char* topic, byte* payload, unsigned int length) {                                  // inicializa mensagem de retorno (tópico, mensagem e comprimento) 
//void callback(char* topic, byte* payload, unsigned int length) {

Serial.print("Mensagem recebida [");                                                            // printa "Mensagem recebida ["
Serial.print(topic); 
Serial.print(payload[0]);                                                                           // printa o nome do tópico que enviou a mensagem
Serial.print("] ");                                                                             // fecha "]"
   for (int i = 0; i < length; i++) {                                                              // Para i = 0 e enquanto i for menor que o comprimento da mensagem, inclui um caractere
      Serial.print((char)payload[i]);                                                               // printa o conteúdo da mensagem
   }
   Serial.println("");                                                                             // quebra de linha 
   if ((char)payload[0] == 'S') {                                                                  // se a mensagem recebida for S (maiúsculo)
      digitalWrite(Sala, HIGH);                                                                     // o LED Sala vai para nível lógico alto
      snprintf (msg, MSG_BUFFER_SIZE, "A luz da sala está ligada");                                 // aparece mensagem indicando que a luz da sala está ligada
      Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
      Serial.println(msg);                                                                          // printa a mensagem
      client.publish("casa/sala", msg);                                                             // publica a mensagem no tópico "casa/sala"
   }
   Serial.println("");                                                                             // quebra de linha
   if ((char)payload[0] == 's') {                                                                  // se a mensagem recebida for s (minúsculo)
      digitalWrite(Sala, LOW);                                                                      // o LED Sala vai para nível lógico baixo 
      snprintf (msg, MSG_BUFFER_SIZE, "A luz da sala está desligada");                              // aparece mensagem indicando que a luz da sala está desligada
      Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
      Serial.println(msg);                                                                          // printa a mensagem
      client.publish("casa/sala", msg);                                                             // publica a mensagem no tópico "casa/sala" 
  }
  Serial.println("");                                                                             // quebra de linha
  if ((char)payload[0] == 'Q') {                                                                  // se a mensagem recebida for Q (maiúsculo)
     digitalWrite(Quarto, HIGH);                                                                   // o LED Quarto vai para nível lógico alto
     snprintf (msg, MSG_BUFFER_SIZE, "A luz do quarto está ligada");                               // aparece mensagem indicando que a luz do quarto está ligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/quarto", msg);                                                           // publica a mensagem no tópico "casa/quarto" 
  }
  Serial.println("");                                                                            // quebra de linha
  if ((char)payload[0] == 'q') {                                                                 // se a mensagem recebida for q (minúsculo)
     digitalWrite(Quarto, LOW);                                                                    // o LED quarto vai para nível lógico baixo 
     snprintf (msg, MSG_BUFFER_SIZE, "A luz do quarto está desligada");                            // aparece mensagem indicando que a luz do quarto está desligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/quarto", msg);                                                           // publica a mensagem no tópico "casa/quarto"
  }
  Serial.println("");                                                                             // quebra de linha
  if ((char)payload[0] == 'C') {                                                                  // se a mensagem recebida for C (maiúsculo)
     digitalWrite(Cozinha, HIGH);                                                                  // o LED cozinha vai para nível lógico alto
     snprintf (msg, MSG_BUFFER_SIZE, "A luz da cozinha está ligada");                              // aparece mensagem indicando que a luz da cozinha está ligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/cozinha", msg);                                                          // publica a mensagem no tópico "casa/cozinha"
  }
  Serial.println("");                                                                             // quebra de linha
  if ((char)payload[0] == 'c') {                                                                 // se a mensagem recebida for c (minúsculo)
     digitalWrite(Cozinha, LOW);                                                                   // o LED cozinha vai para nível lógico baixo
     snprintf (msg, MSG_BUFFER_SIZE, "A luz da cozinha está desligada");                           // aparece mensagem indicando que a luz da cozinha está desligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/cozinha", msg);                                                          // publica a mensagem no tópico "casa/cozinha"
  }
  Serial.println("");                                                                             // quebra de linha
  if ((char)payload[0] == 'B') {                                                                  // se a mensagem recebida for B (maiúsculo)
     digitalWrite(Banheiro, HIGH);                                                                 // o LED banheiro vai para nível lógico alto
     snprintf (msg, MSG_BUFFER_SIZE, "A luz do banheiro está ligada");                             // aparece mensagem indicando que a luz do banheiro está ligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/banheiro", msg);                                                         // publica a mensagem no tópico "casa/banheiro"
  }
  Serial.println("");                                                                            // quebra de linha
  if ((char)payload[0] == 'b') {                                                                 // se a mensagem recebida for b (minúsculo)
     digitalWrite(Banheiro, LOW);                                                                  // o LED banheiro vai para o nível lógico baixo
     snprintf (msg, MSG_BUFFER_SIZE, "A luz do banheiro está desligada");                          // aparece mensagem indicando que a luz do banheiro está desligada
     Serial.print("Publica mensagem: ");                                                           // printa "Publica mensagem:"
     Serial.println(msg);                                                                          // printa a mensagem
     client.publish("casa/banheiro", msg);                                                         // publica a mensagem no tópico "casa/banheiro"
 }
}

void reconnect() {                                                                                // laço para reconexão 
   while (!client.connected()) {                                                                   // enquanto não é estabelecida reconexão
      Serial.print("Aguardando conexão MQTT...");                                                   // printa no serial "Aguardando conexão MQTT..." 
      String clientId = "ESP8266Client";                                       
      clientId += String(random(0xffff), HEX);                                                      // cria uma identidade de cliente aleatória
      if (client.connect(clientId.c_str())) {                                                       // Se é realizada a conexão
         Serial.println("conectado");                                                                // printa "conectado" no serial 
         client.subscribe("casa/publisher");                                                         // e realiza subscrição no tópico "teste_entrada" 
        } else {                                                                                      // caso contrário
         Serial.print("falhou, rc=");                                                                // printa a mensagem "falhou, rc="
         Serial.print(client.state());                                                               // printa status do client
         Serial.println("tente novamente em 5s");                                                    // printa na serial "tente novamente em 5s" 
         delay(5000);                                                                                // atraso de 5s antes de reiniciar o laço void reconnect
      }
   }
}

void loop() {                                                                                     // laço de repetição 
delay(50);
contador++;
   if(contador == 40){ 
      sensors.requestTemperatures();
      temperatura = sensors.getTempCByIndex(0); 
      Serial.print(F("Temperatura: "));                                                             // Printa "Temperatura:"
      Serial.print(temperatura);                                                              // Imprime na serial a medição de temperatura
      Serial.println(F("°C"));                                                                      // Printa C°
      sprintf(msg,"%f",temperatura);                                                          // Salva o valor de temperatura na mensagem
      client.publish("casa/temperatura", msg);                                                      // Publica a mensagem no tópico casa/temperatura
  
      int Analogica_0 = analogRead(A0);   // Ler o pino Analógico A0 onde está o LDR
      tensao = Analogica_0 * (3.3 / 1024.0);
      Serial.print(F("Tensao: "));                                                                 // printa "Umidade:"
      Serial.print(tensao);                                                        // Imprime na serial a medição de umidade
      Serial.println(F("%"));                                                                       // Printa %
      sprintf(msg,"%f",tensao);                                                    // Salva o valor de umidade na mensagem
      client.publish("casa/tensao", msg);                                                          // Publica a mensagem no tópico casa/umidade
      contador = 0;
   }
   if (!client.connected()) {                                                                      // Se o cliente não se conectar
      reconnect();                                                                                  // vai para laço de reconexão
   }
client.loop();                                                                                  // permitir o processamento e a publicação de mensagens e dados
}
