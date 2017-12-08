#http://www.automalabs.com.br/teclado-membrana-de-16-teclas-com-interface-i2c/

#include <Wire.h>
#include <i2ckeypad.h>

#define ROWS 4
#define COLS 3

// Con A0, A1 y A2 del PCF8574A a masa I2C se direcciona 0x38, el PCF8574 se direcciona 0x20.
#define PCF8574A_ADDR 0x38

i2ckeypad kpd = i2ckeypad(PCF8574A_ADDR, ROWS, COLS);

void setup()
{
 Serial.begin(9600);
 Wire.begin();
 kpd.init();

 Serial.print("Testear keypad/PCF8574A I2C port expansor lib Arduino\n\n");
}

void loop()
{
 char key = kpd.get_key();
 if(key != '\0') 
 {
       Serial.print(key);
 }
}

/*
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>
 
#define Endereco_I2C 0x20
 
const byte NumLinhas = 4;
const byte NumColunas = 4;
char hexaKeys[NumLinhas][NumColunas] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[NumLinhas] = {0, 1, 2, 3}; 
byte colPins[NumColunas] = {4, 5, 6, 7}; 
 
Keypad_I2C customKeypad( makeKeymap(hexaKeys), rowPins, colPins, NumLinhas, NumColunas, Endereco_I2C);
 
void setup(){
  customKeypad.begin( );
  Serial.begin(9600);
}
 
void loop(){
  char customKey = customKeypad.getKey();
 
  if (customKey != NO_KEY){
    Serial.println(customKey);
  }
}

*/
