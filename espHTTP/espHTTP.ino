#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

int gaiolaId=1;
String server = "http://172.22.44.76:8080";
String serverTurns =  server +"/api/Turns";
String serverRoda =  server + "/api/Cage"+ String(gaiolaId);;
String dados;
const char* ssid = "AndroidAPE958";                    
const char* senha = "12345678";

// Definição da pinagem
#define RXp2 16
#define TXp2 17 

// Variaveis calculo da roda
int turnCount = 0;
float inicio = 0;
unsigned long lastPressTime = 0;
// unsigned long lastLastPressTime = 0;
float circunferencia = 0;
float pi = 3.14159;
float diametro = 0.7;
float tempoPercorrido=0;
float distanciaPercorrida=0;
float velocidadeMedia=0;

// Definições das funções externas
extern void confiNTP();
extern String getData();

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, senha);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Tentando conectar a rede");
  }

  confiNTP();

  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  circunferencia = pi * diametro;
}

void loop() {
  unsigned long currentTime = millis();
  
  if (Serial2.available() > 0) {
    turnCount = Serial2.read();
    lastPressTime = currentTime;

//  Função para apagar no banco ao apertar no botão    
    // if(turnCount == 0){
      //Apagar informações do banco
      //apagarBanco();
    // }

    else if(turnCount == 1){
        inicio = currentTime;
        // Serial.println("Message Received: ");
        // Serial.println(turnCount);
    } // primeira vez
      
  }

  if(currentTime - lastPressTime > 5000){
    tempoPercorrido = ((lastPressTime - inicio) / 1000);
    
    // if(tempoPercorrido!=0){
    //     Serial.print("Tempo da atividade: ");
    //     Serial.print(tempoPercorrido);
    //     Serial.println(" segundos.");

        getDiametro();

        distanciaPercorrida = circunferencia * turnCount;
        
        // Serial.print("Distância percorrida: ");
        // Serial.print(distanciaPercorrida);
        // Serial.println(" metros.");

        velocidadeMedia = distanciaPercorrida / (tempoPercorrido * 60);
        velocidadeMedia = (velocidadeMedia * 3.6) * 60;
        
        Serial.print("Velocidade média: ");
        Serial.print(velocidadeMedia);
        Serial.println(" km/h");

        postVolta(velocidadeMedia, tempoPercorrido, distanciaPercorrida, turnCount);
    }

    turnCount = 0;
    lastPressTime = currentTime;
    inicio = currentTime;
    Serial.println("Resetting count.");
  } 
}

void getDiametro() {
  DynamicJsonDocument doc(1024);
  HTTPClient http;

  http.begin(serverRoda);
  int httpCode = http.GET(); 

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Resposta: ");
    String payload = http.getString(); // armazena a resposta da requisição

    deserializeJson(doc, payload);
    String diametroS = doc[String("diametro")];
    diametro = diametroS.toFloat();
    circunferencia = pi * diametro;
  } else {
    Serial.print("HTTP POST... Erro. Mensagem de Erro: ");
    Serial.println(http.errorToString(httpCode).c_str()); // Imprime a mensagem de erro da requisição
    delay(500);
  }

  http.end();
}

void postVolta(int velocidadeMedia, int tempoAtividade, float distanciaPercorrida, int quantidade_voltas) {
  String dados = "{\"velocidadeMedia\":  "+ String(velocidadeMedia) +  
                 ",\"data\":   \""+ getData() + 
                 "\", \"gaiolaId\": "+String(gaiolaId)+
                 ", \"tempoAtividade\": "+String(tempoAtividade)+
                 ", \"distanciaPercorrida\": "+String(distanciaPercorrida)+
                 ", \"quantidadeVoltas\":" + String(quantidade_voltas) +
                 "}";
  
  HTTPClient http;
  http.begin(serverTurns);
  http.addHeader("Content-Type", "application/json"); 
  int httpCode = http.POST(dados); 
  Serial.println(dados);

  // if (httpCode == HTTP_CODE_OK) {
    // Serial.println("Resposta: ");
  //   String payload = http.getString();
  //   Serial.println(payload);
  // } 
  // else {
    // Serial.print("HTTP POST... Erro. Mensagem de Erro: ");
    // Serial.println(http.errorToString(httpCode).c_str());
  //   delay(500);
  // }

  http.end();
}

void apagarBanco() {
  turnCount = 0;
  String url = serverTurns + "/" + String(gaiolaId);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json"); 
  int httpCode = http.sendRequest("DELETE");

  // if (httpCode == HTTP_CODE_OK) {
  //   Serial.println("Resposta: ");
  //   String payload = http.getString();
  //   Serial.println(payload);
  // } else {
    // Serial.print("HTTP POST... Erro. Mensagem de Erro: ");
    // Serial.println(http.errorToString(httpCode).c_str());
  //   delay(500);
  // }

  http.end();
}