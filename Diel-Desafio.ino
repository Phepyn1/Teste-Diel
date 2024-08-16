//Diel Energia - Renan D
// Desafio técnico - Estágio de desenvolvimento de firmware

#include <config.h> //arquivo com os dados de configuração
#include <WiFi.h> //lib do WiFi
#include <PubSubClient.h> //Mqtt client  
#include <ArduinoJson.h> //Biblioteca do Json

WiFiClient espClient;
PubSubClient client(espClient);


//variaveis
int luminosidade, potenciometro;
const int pinoLuz = 34;
const int pinoPot = 35;
const char* mqtt_topico = "ESP32/Sensores";
char mqtt_buffer[256];
String comunicacaoSerial;
void setup() {
  
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(115200); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

//incialização do wifi... Os dados da rede estão salvos pelo config.h
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");

  client.setServer(BROKER_IP, BROKER_PORT);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload para string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem: ");
  Serial.println(message);
}





void reconnect() {
  while (!client.connected()) {
    if (client.connect("mqttx_66b7c43347a")) {
      Serial.println("connected");
      client.subscribe("serviço");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println("nova tentativa em 3seg");
      delay(30000);
    }
  }
}

void loop() {
  
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();



  //Leitura dos sensores (Não esquecer de testar com o Serial)
  luminosidade = analogRead(pinoLuz);
  potenciometro = analogRead(pinoPot);

  if (Serial.available() > 0){
     comunicacaoSerial = Serial.readString();
  }

//criar um documento json estático 
  StaticJsonDocument<300> doc;

// criar objeto 'Luz' e o adiciona ao Json
  JsonObject sensor1 = doc.createNestedObject("Luz");
  sensor1["sensor"] = "Luminosidade";
  sensor1["valor"] = luminosidade;

// criar objeto 'Nivel' e o adiciona ao Json
  JsonObject sensor2 = doc.createNestedObject("Nivel");
  sensor2["sensor"] = "potenciometro";
  sensor2["valor"] = potenciometro;

// criar objeto 'Serial' e o adiciona ao Json
  JsonObject sensor3 = doc.createNestedObject("Serial");
  sensor3["Sensor"] = "Serial";
  sensor3["valor"] = comunicacaoSerial;

//Serializa o json ao buffer
  serializeJson(doc, mqtt_buffer);
  Serial.println(mqtt_buffer);
  Serial.println("---.---");

  client.publish(mqtt_topico,mqtt_buffer);
  delay(3000);


}
