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
int temper[5][2];  //массив данных с датчика
float temp = 0.0;
int r = 0;//переменная с номером датчика
int m = 1;
int mass = 0;//переменная со значением от датчика
int massi = 0;
int timi1 = 0;
int timi2 = 0;
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
GTimer myTimer2(MS);

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
  //запуск опроса функции датчика влажности
  if (myTimer.isReady())dht() ;


  //перебор массива и вызов функции дисплея
  if (myTimer1.isReady()) {
    //for (float &mass : temper) {
    mass = temper[r][0];
    massi = temper[r + 1][0];
    disp(mass, r , 0) ;
    disp(massi, r + 1 , 1) ;
    if (r < 4) {
      r = r + 1;
      m = m + 1;
    }
    else {
      r = 1;
      m = 2;
    }
    Serial.print(temper[1][0] * 0.01);
    Serial.print(" ; ");
    Serial.println(temper[2][0] * 0.01);
    Serial.print(temper[1][1]);
    Serial.print(" ; ");
    Serial.println(temper[2][1]);
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
        temper[1][0] = temp * 100;
        timi1 = myTimer2 - timi1;
        temper[1][1] = timi1;

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
        temper[2][0] = temp * 100;
        timi2 += 1;
        temper[2][1] = timi2;

        //disp(temp, 2, 1);
        break;
    }
  }
}


//функция вывода на дисплей принимает значение датчика, номер датчика, номер строчки.
void disp (int data, int n, int stro) {
  //вывод первой строки
  lcd.setCursor(0, stro);
  if (n == 4) {
    lcd.print("Humidity: ");
    lcd.print(data * 0.01);
    lcd.print(F("% "));
  }
  else {
    lcd.print("Temp");
    lcd.print(n);
    lcd.print(":   ");
    lcd.print(data * 0.01);
    lcd.print(F("\337C "));
  }

  //lcd.print("Temp" + n + ":   ");

}


  }*/
//функция датчика флажности и температуры,
void dht () {
  float hum;
  float tem;
  float chk = DHT11.read(DHTPIN);
  // Влажность
  hum = (float)DHT11.humidity, 2;
  temper[4][0] = hum * 100; //передает значение влажность в массив

  // Температура
  tem = (float)DHT11.temperature, 2;
  temper[3][0] = tem * 100; //передает значение температура в массив
}
