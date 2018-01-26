#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Servo.h> 
#include <Wire.h>
#include <Keypad_I2C.h>
#include <Keypad.h>

//Digital 
#define portaReleV D0
#define portaServoP D2
#define melodyPin D3
#define portaServoC D4
#define portaServoJ D5
#define portaBotaoA D6
#define portaBFC D7
#define portaReleA D8
#define portaReleL D9
#define portaBsair D10
#define Chuva D11
#define temp D12
#define ldrD A0

const byte ROWS = 3; 
const byte COLS = 2;

//Valores
int LDRF = 0;
int LDRD = 0;
int presenca = 0;
float temperatura = 0;
int pressao = 0;
int chuva = 0;
int estBFC = 0;
int estBSair = 0;
int estBA = 0;

//Estados: true=aberto/ligado false=fechado/desligado
bool cortina = false;  
bool janela = false;
bool tranca = false;
bool luz1 = false;
bool luz2 = false;
bool alarme = false;
bool ventilador = false;
bool ambientea = false;

//Internet
const char* ssid = "LASSESEMFIO";
const char* password = "l4ss3wifi";
const char* host = "mydream-ufpa-phi.herokuapp.com";
bool online = false;
int count;
String senha;
String passe;
int hora, minuto, horaAcordar, minutoAcordar;

unsigned int localPort = 2390; 

int melodia[] = {660,660,660,510,660,770,380,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,500,500,500,580,660,500,430,380,500,500,500,500,580,660,870,760,500,500,500,500,580,660,500,430,380,660,660,660,510,660,770,380};

int duracaodasnotas[] = {100,100,100,100,100,100,100,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,60,80,60,80,80,80,80,80,80,60,80,60,80,80,80,80,80,60,80,60,80,80,80,80,80,80,100,100,100,100,100,100,100};


IPAddress timeServerIP; 
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

WiFiServer server(5000);
WiFiClientSecure client;
WiFiUDP udp;

//Outras Definições
#define DHTTYPE DHT22
Servo myservoP;
Servo myservoC;
Servo myservoJ;

//Outros Valores


int thershold = 150;
int pausa = 0;

byte grau[8] ={ B00001100, 
                B00010010, 
                B00010010, 
                B00001100, 
                B00000000, 
                B00000000, 
                B00000000, 
                B00000000,}; 

char keys[ROWS][COLS] = {
  {'1','2'},
  {'4','5'},
  {'7','8'}
};

byte rowPins[ROWS] = {D7, D6, D10}; 
byte colPins[COLS] = {D11, D12}; 

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

DHT dht(temp, DHTTYPE);

void porta1(){
  LDRD = analogRead(ldrD);
  Serial.print("Luz de Dentro: ");
  Serial.println(LDRD);
}

void porta2(){
  temperatura = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
}

void porta5(){
  chuva = digitalRead(Chuva);
  Serial.print("Chuva: ");
  Serial.println(chuva);
}

void porta6(){
  estBFC = digitalRead(portaBFC); 
  Serial.print("Estado botão fim de curso");
  Serial.println(estBFC);
}

void porta7(){
  estBSair = digitalRead(portaBsair);
  Serial.print("Estado botão sair: ");
  Serial.println(estBSair);
}

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

void lerSensores(){
   porta1();//dentro
   porta2();//temperatura
   porta5();//chuva
   porta6();//fim de curso
   porta7();//sair
   estBA = digitalRead(portaBotaoA);
   Serial.print("Botão abajur: ");
   Serial.println(estBA);
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
    //Serial.println(line);
    if (line == "\r") {
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
  Serial.println("saiu do while");
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
  if(result == "aaaligado"){
    return true;
  } else{
    return false;
  }
}

String voltaEstados(bool result){
  if(result == true){
    return "aaaligado";
  } else{
    return "desligado";
  }
}

void atualizarEstados(){
  String teste = getRequest("/sensor");
  ambientea = estados(teste.substring(774,783));
  tranca = estados(teste.substring(94,103));
  ventilador = estados(teste.substring(435,444));
  luz1 = estados(teste.substring(548,557));
  luz2 = estados(teste.substring(661,670));
  cortina = estados(teste.substring(323,332));
  janela = estados(teste.substring(208,217));
  alarme = estados(teste.substring(992,1001));
  senha = estados(teste.substring(885,889));  
  horaAcordar = (getRequest("/sensor/hora")).toInt();
  minutoAcordar = (getRequest("/sensor/minuto")).toInt();
}

void atualizarApi(){
  ambientea = estados(getRequest("/sensor/celular"));
  putRequest("/sensor/celular", voltaEstados(ambientea));
  putRequest("/sensor/e_tranca", voltaEstados(tranca));
  putRequest("/sensor/e_vent", voltaEstados(ventilador));
  putRequest("/sensor/e_lamp1", voltaEstados(luz1));
  putRequest("/sensor/e_lamp2", voltaEstados(luz2));
  putRequest("/sensor/e_cortina", voltaEstados(cortina));
  putRequest("/sensor/e_janela", voltaEstados(janela));
  putRequest("/sensor/alarme", voltaEstados(alarme));
}

void putSenha(int n){
  if(count < 4){
     passe += n;
  }
  delay(500); 
  count++;
}

void inserirDigito(){
  char tecla_pressionada = keypad.getKey();
   if (tecla_pressionada != NO_KEY){
       putSenha(tecla_pressionada);
  }
}

void checarSenha(){
  if(count == 4){ //definir sua senha aki
    if(passe == senha){
      myservoP.write(0);
      //tone(melodyPin, 1000);
      delay(1000);
    }else{
      myservoP.write(90);
      //tone(melodyPin, 3000);
    }
    passe = "";
    count = 0;
  }
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
}

void setup (){
  conectar();
  atualizarEstados();
  
  myservoP.attach(portaServoP);
  myservoC.attach(portaServoC);
  myservoJ.attach(portaServoJ);

  myservoJ.write(0);
  myservoP.write(0);
  myservoC.write(0);

  dht.begin();
  
  pinMode(D13, OUTPUT);
  pinMode(D14, OUTPUT);
  pinMode(D15, OUTPUT);
 
  pinMode(portaReleL, OUTPUT);
  pinMode(portaReleA, OUTPUT);
  pinMode(portaReleV, OUTPUT);
  pinMode(melodyPin, OUTPUT);
  pinMode(portaBotaoA, INPUT);
  pinMode(portaBFC, INPUT);
  pinMode(portaBsair, INPUT);

}

//https://cityos.io/tutorial/1958/Use-multiplexer-with-Arduino multiplexador 1
//https://www.arduinoecia.com.br/2017/03/como-usar-pcf8574-expansor-de-portas-i2c.html teclado matricial
//http://www.nadielcomercio.com.br/blog/2015/09/01/ci-pcf8574-expansor-de-portas-io-8-bits-i2c-para-arduino/ multiplexador 2
//http://www.esp8266learning.com/wemos-mini-pcf8574.php 

void loop(){
  relogio();
 
 atualizarEstados();    

    Serial.println("Pelo celular");
    if(luz1){
      Serial.println("lampada liga");
      digitalWrite(portaReleL, HIGH);
    }else{
      Serial.println("lampada nao liga");
      digitalWrite(portaReleL, LOW);
    }

    if(luz2){
      Serial.println("abajur liga");
      digitalWrite(portaReleA, HIGH);
    }else{
      Serial.println("abajur nao liga");
      digitalWrite(portaReleA, LOW);
    }

    if(ventilador){
      Serial.println("ventilador liga");
      digitalWrite(portaReleV, HIGH);
    }else{
      Serial.println("" + ventilador);
      Serial.println("ventilador nao liga");
      digitalWrite(portaReleV, LOW);
    }
    
    if(tranca){
      Serial.println("tranca liga");
      myservoP.write(90);
    }else{
      Serial.println("tranca nao liga");
      myservoP.write(0);
    }

    if(cortina){
      Serial.println("cortina liga");
      myservoC.write(90);
    }else{
      Serial.println("cortina nao liga");
      myservoC.write(0);
    }

    if(janela){
      Serial.println("janela liga");
      myservoJ.write(180);
    }else{
      Serial.println("janela nao liga");
      myservoJ.write(0);
    }   
  /*} else{
    Serial.println("Pelo ambiente");
    if(hora > 20 || hora < 7){
        myservoC.write(90);
        cortina = false;
        digitalWrite(portaReleL, LOW);
        luz1 = false;
     } else{
        if(LDRD < 600){ //tem mais luz fora
          myservoC.write(90);  // abre a cortina
          cortina = true;
        }

        if(LDRD < 550 && (hora > 19 || hora < 7)){
          digitalWrite(portaReleL, HIGH);
          luz1 = true;
        } else{
          digitalWrite(portaReleL, LOW);
          luz1 = false;
        }
     }

     if(chuva){
       myservoJ.write(180);
       janela = false;
     }else{
       myservoJ.write(0);
       janela = true;
     }

     if(temperatura >= 26){
       digitalWrite(portaReleV, HIGH);
       ventilador = true;
     }else{
       digitalWrite(portaReleV, LOW);
       ventilador = false;
     }
  }*/
 
  if(alarme && horaAcordar == hora && minutoAcordar == minuto){
    Serial.println("oi");
    for (int nota = 0; nota < 156; nota++) {
      int duracaodanota = duracaodasnotas[nota];
      tone(melodyPin, melodia[nota],duracaodanota);
      int pausadepoisdasnotas[] ={150,300,300,100,300,550,575,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,150,300,350,150,350,150,300,150,600,150,300,350,150,150,550,325,600,150,300,350,150,350,150,300,150,600,150,300,300,100,300,550,575};
      delay(pausadepoisdasnotas[nota]);
    }
      noTone(melodyPin);
  }

  if(estBSair){
    myservoP.write(0);
    tranca = true;
  }

  if(estBFC){
    myservoP.write(90);
    tranca = false;
  }
  
  if(estBA){
    digitalWrite(portaReleA, HIGH);
    luz2 = true;
  }else{
    digitalWrite(portaReleA, LOW);
    luz2 = false;
  }
}

