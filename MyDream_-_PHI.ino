#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "meuwifi";
const char* password = "minhasenha";
const char* host = "mydream-ufpa-phi.herokuapp.com";
bool e_cel, e_tranca, e_vent, e_lamp1, e_lamp2, e_cortina, e_janela;
String senha = "1234";
bool online = false;

WiFiServer server(5000);
WiFiClientSecure client;

void conectar(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.println(host);
  server.begin();
  if (!client.connect(host, 443)) {
    Serial.println("connection failed");
    return;
  }
}

String getRequest(String quem){
  if ( !client.connect("mydream-ufpa-phi.herokuapp.com", 443) ) {
    online = false;
    Serial.println("conexao falhou");
    return "connection failed";
  }

  online = true;
  
  client.print(String("GET ") + quem + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
 
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
  return line.substring(1, line.length() - 1);  
}

void putRequest(String quem, String estado){
  if ( !client.connect("mydream-ufpa-phi.herokuapp.com", 443) ) {
    Serial.println("conexao falhou");
    online = false;
    return;
  }

  online = true;

  String data = "{\"estado\":\"" + estado + "\"}";

  client.print(String("PUT ") + quem + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 //"Connection: close\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + data.length() + "\r\n" +
                 "\r\n" + // This is the extra CR+LF pair to signify the start of a body
                 data + "\n");
}

bool estados(String result){
  if(result == "ligado"){
    return true;
  } else{
    return false;
  }
}

String voltaEstados(bool result){
  if(result == true){
    return "ligado";
  } else{
    return "desligado";
  }
}

void atualizarEstados(){
  e_cel = estados(getRequest("/sensor/celular"));
  e_tranca = estados(getRequest("/sensor/e_tranca"));
  e_vent = estados(getRequest("/sensor/e_vent"));
  e_lamp1 = estados(getRequest("/sensor/e_lamp1"));
  e_lamp2 = estados(getRequest("/sensor/e_lamp2"));
  e_cortina = estados(getRequest("/sensor/e_cortina"));
  e_janela = estados(getRequest("/sensor/e_janela"));
  senha = getRequest("/sensor/senha");    
}

void atualizarApi(){
  putRequest("/sensor/celular", voltaEstados(e_cel));
  putRequest("/sensor/e_tranca", voltaEstados(e_tranca));
  putRequest("/sensor/e_vent", voltaEstados(e_vent));
  putRequest("/sensor/e_lamp1", voltaEstados(e_lamp1));
  putRequest("/sensor/e_lamp2", voltaEstados(e_lamp2));
  putRequest("/sensor/e_cortina", voltaEstados(e_cortina));
  putRequest("/sensor/e_janela", voltaEstados(e_janela));
}

void setup (){
  conectar();
}
 
void loop() { 
  if(getRequest("/sensor/celular") == "desligado"){
    atualizarApi();
  } else{
    atualizarEstados();    
  }
  
  if(!online){
    //interaçoes aki
    //fazer flag pra put request a cada interaçao  
    
  } else{
    //checa todos os estados e faz os modulos obedecerem

  }
}
