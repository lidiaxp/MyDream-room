#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Servo.h> 
#include <Wire.h>
#include <Keypad_I2C.h>
#include <Keypad.h>

//Para tocar Mario
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

//Analógico
/*#define portaLDRF 0 
#define portaLDRD 1 
#define portaDHT 2
#define portaPiezo 3*/

//Digital
//#define portaPIR D0 
#define portaReleL D9
#define portaReleA D8
#define portaReleV D0
#define melodyPin D3
//#define portaChuva D5
//#define portaLedRed D4
//#define portaLedGreen D5
#define portaServoP D2
#define portaServoC D4
#define portaServoJ D5
//#define portaBotaoFC D6
//#define portaBotaoSair D7
#define portaBotaoA D8

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
const char* ssid = "LDI";
const char* password = "somenteldimaiusculo";
const char* host = "mydream-ufpa-phi.herokuapp.com";
bool online = false;
int count;
String senha;
String passe;
int hora, minuto, horaAcordar, minutoAcordar;

unsigned int localPort = 2390; 

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

//int i2caddress = 0x20;
//Keypad_I2C kpd = Keypad_I2C( makeKeymap(teclas), PinsFilas, PinsColumnas, FILAS, COLUMNAS, i2caddress );

void porta0(){
  digitalWrite(D13, LOW);//A
  digitalWrite(D14, LOW);//B
  digitalWrite(D15, LOW);//C
  delay(50);
  pinMode(A0, INPUT);
  LDRF = analogRead(A0);
}

void porta1(){
  digitalWrite(D13, HIGH);//A
  digitalWrite(D14, LOW);//B
  digitalWrite(D15, LOW);//C
  delay(50);
  pinMode(A0, INPUT);
  LDRD = analogRead(A0);
}

void porta2(){
  digitalWrite(D13, LOW);
  digitalWrite(D14, HIGH);
  digitalWrite(D15, LOW);
  delay(50);
  DHT dht(A0, DHTTYPE);
  dht.begin();
  temperatura = dht.readTemperature();
}

void porta3(){
  digitalWrite(D13, HIGH);
  digitalWrite(D14, HIGH);
  digitalWrite(D15, LOW);
  delay(50);
  pinMode(A0, INPUT);
  pressao = analogRead(A0);
}

void porta4(){
  digitalWrite(D13, LOW);
  digitalWrite(D14, LOW);
  digitalWrite(D15, HIGH);
  delay(50);
  pinMode(A0, INPUT);
  presenca = digitalRead(A0);
}

void porta5(){
  digitalWrite(D13, HIGH);
  digitalWrite(D14, LOW);
  digitalWrite(D15, HIGH);
  delay(50);
  pinMode(A0, INPUT);
  chuva = analogRead(A0);
}

void porta6(){
  digitalWrite(D13, LOW);
  digitalWrite(D14, HIGH);
  digitalWrite(D15, HIGH);
  delay(50);
  pinMode(A0, INPUT);
  estBFC = analogRead(A0); 
}

void porta7(){

  digitalWrite(D13, HIGH);
  digitalWrite(D14, HIGH);
  digitalWrite(D15, HIGH);
  delay(50);
  pinMode(A0, INPUT);
  estBSair = analogRead(A0);
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
   porta0();
   porta1();
   porta4();
   porta2();
   porta3();
   porta5();
   porta6();
   porta7();
   estBA = digitalRead(portaBotaoA);
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

//Mario main theme melody
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};
int song = 0;

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2;
  long numCycles = frequency * length / 1000; 
  for (long i = 0; i < numCycles; i++) { 
    digitalWrite(targetPin, HIGH); 
    delayMicroseconds(delayValue); 
    digitalWrite(targetPin, LOW); 
    delayMicroseconds(delayValue); 
  }
}

void sing(int s) {
  song = s;
  if (song == 2) {
    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / underworld_tempo[thisNote];
      buzz(melodyPin, underworld_melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      buzz(melodyPin, 0, noteDuration);
    }
  } else {
    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / tempo[thisNote];
      buzz(melodyPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      buzz(melodyPin, 0, noteDuration);
    }
  }
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
  ambientea = estados(getRequest("/sensor/celular"));
  Serial.println("" + ambientea);
  tranca = estados(getRequest("/sensor/e_tranca"));
  ventilador = estados(getRequest("/sensor/e_vent"));
  luz1 = estados(getRequest("/sensor/e_lamp1"));
  luz2 = estados(getRequest("/sensor/e_lamp2"));
  cortina = estados(getRequest("/sensor/e_cortina"));
  janela = estados(getRequest("/sensor/e_janela"));
  alarme = estados(getRequest("/sensor/alarme"));
  senha = getRequest("/sensor/senha");  
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
       tone(melodyPin, 2000);
  }
}

void checarSenha(){
  if(count == 4){ //definir sua senha aki
    if(passe == senha){
      myservoP.write(0);
      tone(melodyPin, 1000);
      delay(1000);
    }else{
      myservoP.write(90);
      tone(melodyPin, 3000);
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
  //delay(60000);
}

void setup (){
  conectar();
  senha = getRequest("/sensor/senha");
  ambientea = estados(getRequest("/sensor/celular"));
  
  //kpd.begin();
  
  myservoP.attach(portaServoP);
  myservoC.attach(portaServoC);
  myservoJ.attach(portaServoJ);

  myservoJ.write(0);
  myservoP.write(90);
  myservoC.write(0);
  
  pinMode(D13, OUTPUT);
  pinMode(D14, OUTPUT);
  pinMode(D15, OUTPUT);
 
  pinMode(portaReleL, OUTPUT);
  pinMode(portaReleA, OUTPUT);
  pinMode(portaReleV, OUTPUT);
  pinMode(melodyPin, OUTPUT);
  //pinMode(portaChuva, INPUT);
  //pinMode(portaLedRed, OUTPUT);
  //pinMode(portaLedGreen, OUTPUT);
  //pinMode(portaBotaoFC, OUTPUT);
  //pinMode(portaBotaoSair, OUTPUT);
  pinMode(portaBotaoA, OUTPUT);
}

//https://cityos.io/tutorial/1958/Use-multiplexer-with-Arduino multiplexador 1
//https://www.arduinoecia.com.br/2017/03/como-usar-pcf8574-expansor-de-portas-i2c.html teclado matricial
//http://www.nadielcomercio.com.br/blog/2015/09/01/ci-pcf8574-expansor-de-portas-io-8-bits-i2c-para-arduino/ multiplexador 2
//http://www.esp8266learning.com/wemos-mini-pcf8574.php 

void loop(){
  relogio();
  inserirDigito();
  checarSenha();
  lerSensores();

  //if(pausa > 15){
    if(!ambientea){
      atualizarApi();
    } else{
      atualizarEstados();    
    }
    pausa = 0;
  //}
  
   //pausa++;

   if(ambientea){
    //pelo celular
    Serial.println("celular");
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
      myservoP.write(0);
    }else{
      Serial.println("tranca nao liga");
      myservoP.write(90);
    }

    if(cortina){
      Serial.println("cortina liga");
      myservoC.write(180);
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
  }else{
    Serial.println("ambiente");
    if((LDRF - 100) > LDRD){
        myservoC.write(0);
        cortina = true;
    }else{
       if(!pressao > thershold){
          digitalWrite(portaReleL, HIGH);
          luz1 = true;
       }
     }

     if(pressao > thershold){
        myservoC.write(90);
        cortina = false;
        digitalWrite(portaReleL, LOW);
        luz1 = false;
      }

      if(chuva){
        myservoJ.write(180);
        janela = false;
      }else{
        myservoJ.write(0);
        janela = true;
      }

      if(temperatura >= 27){
        digitalWrite(portaReleV, HIGH);
        ventilador = true;
      }else{
        digitalWrite(portaReleV, LOW);
        ventilador = false;
      }
  //parte do dane-se ambiente
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
  
    if(alarme && horaAcordar == hora && minutoAcordar == minuto){
      sing(1);
      sing(1);
      sing(2);
    }
  }
}

/* 
void loop() {
  //Serial.println("No loop");
  relogio();
  inserirDigito();
  checarSenha();
  lerSensores();

  if(pausa > 15){
    if(!ambientea){
      atualizarApi();
    } else{
      atualizarEstados();    
    }
    pausa = 0;
  }
  
   pausa++;
   
   if(ambientea){
    //pelo celular
    if(luz1){
      digitalWrite(portaReleL, HIGH);
    }else{
      digitalWrite(portaReleL, LOW);
    }

    if(luz2){
      digitalWrite(portaReleA, HIGH);
    }else{
      digitalWrite(portaReleA, LOW);
    }

    if(ventilador){
      digitalWrite(portaReleV, HIGH);
    }else{
      digitalWrite(portaReleV, LOW);
    }
    
    if(tranca){
      myservoP.write(0);
    }else{
      myservoP.write(90);
    }

    if(cortina){
      myservoC.write(0);
    }else{
      myservoC.write(90);
    }

    if(janela){
      myservoJ.write(0);
    }else{
      myservoJ.write(180);
    }
  }else{
    //pelo ambiente
    //if(presenca){
    if((LDRF - 100) > LDRD){
        myservoC.write(0);
        cortina = true;
    }else{
       if(!pressao > thershold){
          digitalWrite(portaReleL, HIGH);
          luz1 = true;
       }
     }

      if(pressao > thershold){
        myservoC.write(90);
        cortina = false;
        digitalWrite(portaReleL, LOW);
        luz1 = false;
      }

      if(chuva){
        myservoJ.write(180);
        janela = false;
      }else{
        myservoJ.write(0);
        janela = true;
      }

      if(temperatura >= 27){
        digitalWrite(portaReleV, HIGH);
        ventilador = true;
      }else{
        digitalWrite(portaReleV, LOW);
        ventilador = false;
      }
    /*}else{
      myservoC.write(90);
      myservoP.write(90);
      myservoJ.write(180);
      digitalWrite(portaReleV, LOW);
      digitalWrite(portaReleL, LOW);
      digitalWrite(portaReleA, LOW);
      cortina = false;
      tranca = false;
      janela = false;
      ventilador = false;
      luz1 = false;
      luz2 = false;
     }
  }*/
  /*
  //parte do dane-se ambiente
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
  
  if(alarme && horaAcordar == hora && minutoAcordar == minuto){
    sing(1);
    sing(1);
    sing(2);
  }
}*/
  
/*void debug(){
  Serial.print("LDR de fora: ");
  Serial.println(LDRF);
  Serial.print("LDR de dentro: ");
  Serial.println(LDRD);
  Serial.print("Presença: ");
  Serial.println(presenca);
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Piezo: ");
  Serial.println(pressao);
  Serial.print("Chuva: ");
  Serial.println(chuva);
  delay(1200);
}*/

/*
Para melhorar o projeto:
Usar Thread
Entender o endereço do PCF8574
Usar JSON no arduino
*/
