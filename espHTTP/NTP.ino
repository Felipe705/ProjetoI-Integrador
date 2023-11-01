#include "time.h"

// // Variaveis de configurações da hora
const char* ntpServer = "time.windows.com";
const long  gmtOffset_sec = -3;
const int   daylightOffset_sec = 0;

// // Definições das funções
void confiNTP();
String getData();

// Função para configurar data e hora
void confiNTP() {
  delay(500);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(500);
}

// Função para retornar a data e hora
String getData(){
  String data = "";

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
  }

  int ano = timeinfo.tm_year;
  int mes = timeinfo.tm_mon;
  int dia = timeinfo.tm_mday;
  int horas = timeinfo.tm_hour;
  int minuto = timeinfo.tm_min;
  int segundo = timeinfo.tm_sec;

  if(horas>3) {
    horas -= 3;
  } else {
    horas += 21;
    if(horas == 0) {
      dia++;
    }
    dia -= 1;
  }

  mes +=1;
  ano = (ano+1900);

  data = conrverte(ano)+"-"+conrverte(mes)+"-"+conrverte(dia)+"T"+conrverte(horas)+":"+conrverte(minuto)+":"+conrverte(segundo)+".000Z";

  return data;
}

// Função para formatar campo
String conrverte(int num) {
  String final="";

  if(num < 10) {
    final += "0"+ String(num);
  } else {
    final = String(num);
  }

  return final;
} 