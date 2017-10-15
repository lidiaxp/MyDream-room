#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#define I2CADDR 0x38

const char* ssid = "meuwifi";
const char* password = "minhasenha";
const char* host = "mydream-ufpa-phi.herokuapp.com";
bool e_cel, e_tranca, e_vent, e_lamp1, e_lamp2, e_cortina, e_janela, e_alarme, ambientea;
int count;
String senha;
String passe;
bool online = false;
int hora, minuto, horaAcordar, minutoAcordar;

unsigned int localPort = 2390; 

IPAddress timeServerIP; 
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

WiFiServer server(5000);
WiFiClientSecure client;
WiFiUDP udp;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6}; //connect to the column pinouts of the keypad

Keypad_I2C kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574 );


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

  udp.begin(localPort);

  Serial.print("connecting to ");
  Serial.println(host);
  server.begin();
  if (!client.connect(host, 443)) {
    Serial.println("connection failed");
    return;
  }
}

unsigned long sendNTPpacket(IPAddress& address){
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011; 
  packetBuffer[1] = 0;   
  packetBuffer[2] = 6;     
  packetBuffer[3] = 0xEC;  
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  udp.beginPacket(address, 123); 
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
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
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + data.length() + "\r\n" +
                 "\r\n" + 
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
  e_alarme = estados(getRequest("/sensor/alarme"));
  senha = getRequest("/sensor/senha");  
  horaAcordar = atoi(getRequest("/sensor/hora"));
  minutoAcordar = atoi(getRequest("/sensor/minuto"));
  ambientea = estados(getRequest("/sensor/ambientea"));
}

void atualizarApi(){
  putRequest("/sensor/celular", voltaEstados(e_cel));
  putRequest("/sensor/e_tranca", voltaEstados(e_tranca));
  putRequest("/sensor/e_vent", voltaEstados(e_vent));
  putRequest("/sensor/e_lamp1", voltaEstados(e_lamp1));
  putRequest("/sensor/e_lamp2", voltaEstados(e_lamp2));
  putRequest("/sensor/e_cortina", voltaEstados(e_cortina));
  putRequest("/sensor/e_janela", voltaEstados(e_janela));
  putRequest("/sensor/alarme", voltaEstados(e_alarme));
}

void relogio(){
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP);
  delay(1000);
  int cb = udp.parsePacket();
  if (!cb) {
    //sem pacote de dados
  }else {
    //com pacote
    udp.read(packetBuffer, NTP_PACKET_SIZE); 
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears; 
    hora = ((epoch  % 86400L) / 3600) - 3; 
    minuto = (epoch  % 3600) / 60;    
    Serial.print("hora: ");
    Serial.println(hora);
    Serial.print("minuto: ");
    Serial.println(minuto);
  }
  //delay(60000);
}

void putSenha(int n){
  if(count < 4){
     passe += n;
  }
  delay(500); 
  count++;
}

void inserirDigito(){
  char tecla_pressionada = kpd.getKey();
   if (tecla_pressionada){
       putSenha(tecla_pressionada);
  }
}

void checarSenha(){
  if(count == 4){ //definir sua senha aki
    if(passe == senha){
      //acende led verde e abre a porta
    }else{
      //acende led vermelho e fecha a porta
    }
    passe = "";
    count = 0;
  }
}

void setup (){
  conectar();
  senha = getRequest("/sensor/senha");
  Wire.begin( );   
  kpd.begin( makeKeymap(keys) );
}
 
void loop() { 
  relogio();
  //checa a luminaria
  inserirDigito();
  checarSenha();
  if(getRequest("/sensor/celular") == "desligado"){
    atualizarApi();
    //continua o codigo daki
  } else{
    atualizarEstados();    
  }
  //a parte do ambiente é daki
  
  if(!online){
    //interaçoes aki
    //fazer flag pra put request a cada interaçao  
    
  } else{
    //checa todos os estados e faz os modulos obedecerem

  }
}
