/*
   Скетч для базовой станции
*/
#define RADIO_DATA 2
#define RADIO_BUF_SIZE 10
#define G433_SPEED 1000
#define RADIO_VCC  3
#define DHTPIN 8
#define DHT_VCC  7
#define DHT_GND  6
float temper[5];  //массив данных с датчика
float temp = 0.0;
int r = 0;
int m = 1;//переменная с номером датчика
float mass = 0;
float massi = 0;//переменная со значением от датчика
#include <dht11.h> // подключаем библиотеку для DHT11
#include <Gyver433.h>
#include <GyverNTC.h>
#include <microDS18B20.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LCD_I2C.h>
dht11 DHT11;

LCD_I2C lcd(0x27); // Default address of most PCF8574 modules, change according

Gyver433_RX <RADIO_DATA, RADIO_BUF_SIZE, G433_XOR> rx;
GyverNTC ntc (0, 15000, 6430, 25, 15000);
MicroDS18B20 <0> ds;
#include "GyverTimer.h"
GTimer myTimer(MS);
GTimer myTimer1(MS); // создать миллисекундный таймер


void setup() {
  Serial.begin(9600);
  pinMode(RADIO_VCC, OUTPUT);// Питание радио
  pinMode(DHT_VCC, OUTPUT);// Питание DHT
  pinMode(DHT_GND, OUTPUT);// Питание радио
  digitalWrite(RADIO_VCC, HIGH);        // Запитываем
  digitalWrite(DHT_VCC, HIGH);        // Запитываем
  digitalWrite(DHT_GND, LOW);        // Запитываем
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
  // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
  myTimer.setInterval(1000);   // настроить интервал
  myTimer1.setInterval(3000);   // настроить интервал

}

void loop() {
  //
  if (myTimer.isReady())dht() ;

  //перебор массива и фызов функции дисплея
  if (myTimer1.isReady()) {
    //for (float &mass : temper) {
    mass = temper[r];
    massi = temper[r + 1];
    disp(mass, r , 0) ;
    disp(massi, r+1 , 1) ;
    if (r < 5) {
      r = r + 1;
      m = m + 1;
    }
    else {
      r = 1;
      m = 2;
    }
    // Serial.println(r);
    //Serial.println(mass);
  }
  //опрос радиоприемника и запись значени в массив
  if (rx.tickWait()) {
    switch (rx.buffer[0]) { // Получаем адрес модуля
      case 0xA1:            // Кнопка
        Serial.println(rx.buffer[1] ? "Button released" : "Button pressed");
        break;
      case 0xA2:            // Термистор
        //Serial.print("NTC1: ");
        //Serial.print(ntc.computeTemp(rx.buffer[1] << 8 | rx.buffer[2]));
        //Serial.println("°C");
        temp = ntc.computeTemp(rx.buffer[1] << 8 | rx.buffer[2]);
        temper[1] = temp;
        // disp(temp + 0.4, 1, 0);
        break;
      case 0xA3:            // Фоторезистор
        Serial.print("Photo value: ");
        Serial.println(rx.buffer[1] << 8 | rx.buffer[2]);
        break;
      case 0xA4:            // PIR детектор
        Serial.println("Pir detected: ");
        break;
      case 0xA5:            // DS18B20
        Serial.print("DS18B20: ");
        Serial.print(ds.calcRaw(rx.buffer[1] << 8 | rx.buffer[2]));
        Serial.println(" *C");
        break;
      case 0xA6:            // dth11
        break;
      case 0xA7:            // Термистор2
        temp = ntc.computeTemp(rx.buffer[1] << 8 | rx.buffer[2]);
        temper[2] = temp;
        //disp(temp, 2, 1);
        break;
    }
  }
}


//функция вывода на дисплей принимает значение датчика, номер датчика, номер строчки.
void disp (float data, int n, int stro) {
  //вывод первой строки
  int ny;
  float datay;
  lcd.setCursor(0, stro);
  lcd.print("Temp");
  lcd.print(n);
  lcd.print(":   ");
  //lcd.print("Temp" + n + ":   ");
  lcd.print(data);
  lcd.print(F("\337C "));
}
/*
  void dht () {
  float hum;
  float chk = DHT11.read(DHTPIN);
  // Влажность
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  hum = (float)DHT11.humidity, 2;
  lcd.print(hum);
  temper[9] = hum;
  lcd.print(" % ");

  // Температура
  lcd.setCursor(0, 0);
  lcd.print("Temp:    ");
  lcd.print((float)DHT11.temperature, 2);
  lcd.print("\337C ");

  }*/
//функция датчика флажности и температуры,
void dht () {
  float hum;
  float tem;
  float chk = DHT11.read(DHTPIN);
  // Влажность
  hum = (float)DHT11.humidity, 2;
  temper[3] = hum; //передает значение влажность в массив

  // Температура
  tem = (float)DHT11.temperature, 2;
  temper[4] = tem;//передает значение температура в массив
}
