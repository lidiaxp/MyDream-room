#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "meuwifi";
const char* password = "minhasenha";
String host = "https://mydream-ufpa-phi.herokuapp.com/sensor";
bool e_cel, e_tranca, e_vent, e_lamp1, e_lamp2, e_cortina, e_janela = false;
String senha = "1234";

WiFiServer server(5000);
WiFiClient client;


void conectar(){
  Serial.begin(115200);
  delay(10);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();
  Serial.println("Server started");  

  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void conexaoInicio(){
  client = server.available();
  if (!client) {
    return;
  }
 
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
}

String getRequest(String quem){
  Serial.println("\nPegando estado\n");
  if ( !client.connect("https://mydream-ufpa-phi.herokuapp.com/sensor", 5000) ) {
    Serial.println("conexao falhou");
    return "connection failed";
  }
  
  client.println("GET " + quem + " HTTP/1.1");
  client.print("Host: ");
  client.println("https://mydream-ufpa-phi.herokuapp.com/sensor");
  client.println("Connection: close");
  client.println();
 
  while (client.connected()) {
    String data = client.readStringUntil('\n'); 
    Serial.println(data);
    if (data == "\r") {    
      break;
    }
  }
  String data = client.readStringUntil('\n');
  Serial.println("Dados recebidos\n");
  Serial.println(data);  
  return data;  
}

void conexaoFim(){
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

void putRequest(String quem, String estado){
  if ( !client.connect("https://mydream-ufpa-phi.herokuapp.com/sensor", 5000) ) {
    Serial.println("conexao falhou");
  }
  
  client.println("PUT " + quem + " HTTP/1.1");
  client.print("Host: ");
  client.println("https://mydream-ufpa-phi.herokuapp.com/sensor");
  client.println("Connection: close");
  client.println("{estado:" + estado + "}");
  client.println();
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
  e_cel = estados(getRequest("/celular"));
  e_tranca = estados(getRequest("/e_tranca"));
  e_vent = estados(getRequest("/e_vent"));
  e_lamp1 = estados(getRequest("/e_lamp1"));
  e_lamp2 = estados(getRequest("/e_lamp2"));
  e_cortina = estados(getRequest("/e_cortina"));
  e_janela = estados(getRequest("/e_janela"));
  senha = getRequest("/senha");    
}

void atualizarApi(){
  putRequest("/celular", voltaEstados(e_cel));
  putRequest("/e_tranca", voltaEstados(e_tranca));
  putRequest("/e_vent", voltaEstados(e_vent));
  putRequest("/e_lamp1", voltaEstados(e_lamp1));
  putRequest("/e_lamp2", voltaEstados(e_lamp2));
  putRequest("/e_cortina", voltaEstados(e_cortina));
  putRequest("/e_janela", voltaEstados(e_janela));
}

void setup (){
  conectar();
}
 
void loop() { 
  //interaçoes aki
  //fazer flag pra put request a cada interaçao
  
  conexaoInicio();
  
  if(getRequest("/celular") == "desligado"){
    atualizarApi();
  } else{
    atualizarEstados();    
  }
  conexaoFim();
}
