//Bibliotecas
#include <Wire.h> //Comunicacao I2C
#include <Adafruit_GFX.h> //Graficos
#include <MCUFRIEND_kbv.h> //Hardware ST7735
#include <OneWire.h> //Biblioteca 1Wire 
#include <DallasTemperature.h> //Biblioteca DS18B20
#include <SoftwareSerial.h> //Biblioteca HC05

//DS18B20
#define ONE_WIRE_BUS 21
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
uint8_t sensor1[8] = { 0x28, 0xF4, 0x67, 0x79, 0x97, 0x10, 0x03, 0x97 };
uint8_t sensor2[8] = { 0x28, 0x92, 0x96, 0x79, 0x97, 0x10, 0x03, 0x8D };

//TFT
MCUFRIEND_kbv tft; //Indicar para a biblioteca o TFT
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <FreeDefaultFonts.h>
#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define GREY    0x8410

//Bluetooth 
SoftwareSerial bluetooth(19, 18);

//Variaveis
float TS1; //Temperatura do S1
float TS2; //Temperatura do S2
int T = 30; //Variavel de tempo
float TIP; //Temperatura inicial da pele
float TIM; //Temperatura inicial da mancha
int CP; //comparacao da pele
int CM; //comp da mancha

void setup(void)
{
  Serial.begin(9600); //Comunicacao serial em 9600baud
  uint16_t ID = tft.readID(); //16 bits para a leitura do TFT
  if (ID == 0xD3) ID = 0x9481; //ID do ST7735
  sensors.begin();
  tft.begin(ID); //Inicia a comunicacao com o ST7735  
  tft.setRotation(0); //Rotacao para portrait
  bluetooth.begin(9600);
}

void loop(void)
{
  bemvindo();   
}

void bemvindo() {
  tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v2.1");
  showmsgXY(0, 40, 1, &FreeSans9pt7b, "Projeto de TCC ETEC HAS");
  pbranco();
  tft.setCursor(0, 80);
  tft.print("Bem-vindo ao Scanner de");
  tft.setCursor(0, 100);
  tft.print("Melanoma");
  delay(3000);
  tft.setCursor(0, 160);
  tft.print("Medindo a temperatura inicial");  
  tft.setCursor(0, 180);
  tft.print("em 5 segundos...");  
  delay(7000);
  tinicial();
}

void tinicial() {
  tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v2.1");  
  showmsgXY(0, 80, 1, &FreeSans9pt7b, "Temperatura inicial (pele)");
  tft.setCursor(20, 100);
  getTempSensor(sensor1);
  pbranco();
  TIP = TS1;
  tft.print(TS1);
  showmsgXY(0, 120, 1, &FreeSans9pt7b, "Temperatura inicial (mancha)");
  tft.setCursor(20, 140);
  getTempSensor(sensor2);
  pbranco();
  TIM = TS2;
  tft.print(TS2);
  delay(7000);
  esfrie();
}

void esfrie() {
  tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v2.1");  
  pbranco();
  tft.setCursor(0, 50);
  tft.print("Esfrie a area do exame");
  tft.setCursor(0, 70);
  tft.print("com equipamento apropriado.");
  showmsgXY(0, 100, 1, &FreeSans9pt7b, "Prosseguindo em:");
  tft.setCursor(50, 120);
  pbranco();
  tft.print("segundos");
  contagem();
}


void contagem() {
  if (T ==  0)  {
    coloque();
} else {
  pbranco();
  tft.setCursor(20, 120);
  tft.print(T);
  delay(1000);
  tft.setCursor(20, 120);
  tft.setTextColor(BLACK);
  tft.print(T);
  T--;
  contagem();
}
}

void coloque() {
  tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v2.1");
  pbranco();
  tft.setCursor(0, 50);
  tft.print("Coloque os sensores nas");
  tft.setCursor(0, 70);
  tft.print("posicoes iniciais para o inicio");
  tft.setCursor(0, 90);
  tft.print("do diagnostico");
  delay(10000); 
  diag();
}

void diag() {
  tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v2.1");
  showmsgXY(0, 40, 1, &FreeSans9pt7b, "Temperatura inicial (pele)");
  pbranco();
  tft.setCursor(20, 60);
  tft.print(TIP);
  showmsgXY(0, 80, 1, &FreeSans9pt7b, "Temperatura da pele");
  showmsgXY(0, 120, 1, &FreeSans9pt7b, "Temperatura inicial (mancha)");
  pbranco();
  tft.setCursor(20, 140);
  tft.print(TIM);
  showmsgXY(0, 160, 1, &FreeSans9pt7b, "Temperatura da mancha"); 
  delay(10000);
  calc();
}

void calc() {
  pbranco();
  tft.setCursor(20, 100);
  getTempSensor(sensor1);
  tft.print(TS1);
  tft.setCursor(20, 180);
  getTempSensor(sensor2);
  tft.print(TS2);
  //
  delay(1000);
  ppreto();
  tft.setCursor(20, 100);
  tft.print(TS1);
  tft.setCursor(20, 180);
  tft.print(TS2);
  //
  if (TS1 <= TIP) {
      CP++;
    } if (TS2 <= TIM) {
      CM++;
    } if (TS1 >= TIP && TS2 >= TIM) {
      int X = CM + CP;
      int Y = CP/X;
      int Z = Y*100;
       tft.fillScreen(BLACK);
  showmsgXY(0, 20, 1, &FreeSans9pt7b, "Scanner de Melanoma v0.5i");
  showmsgXY(0, 40, 1, &FreeSans9pt7b, "Resultado");
  pbranco();
  tft.setCursor(0,70);
  tft.print("A probabilidade de a");
  tft.setCursor(0,90);
  tft.print("mancha ser de origem");
  tft.setCursor(0,110);
  tft.print("cancerigena é:");
  tft.setCursor(0,130);
  tft.print(Z);
  tft.setCursor(30,130);
  tft.print("%");
  bluetooth.print(Z);
  exit(0);
      }
      calc();
}

  void pbranco()
{
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
}

  void ppreto()
{
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
}

void getTempSensor(DeviceAddress deviceAddress)
{
  float a = sensors.getTempC(deviceAddress);
  if (deviceAddress == sensor1) {
    TS1 = a;
    } if (deviceAddress == sensor2) {
      TS2 = a;
      }
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.drawFastHLine(0, y, tft.width(), WHITE);
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(GREEN);
    tft.setTextSize(sz);
    tft.print(msg);
    delay(10);
}
