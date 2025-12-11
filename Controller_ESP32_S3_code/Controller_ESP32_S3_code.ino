/*
Настройки использованной платы ESP32S3 Dev Module 
                                                                             USB CDC On Boot:"Disabled"
                                                                             CPU Frequency: "240MHz (WiFi)"
                                                                             Core Debug Level: "None"
                                                                             USB DFU On Boot: "Enabled (Requires USB-OTG Mode)"
                                                                             Erase All Flash Before Sketch Upload: "Disabled"
                                                                             Events Run On: "Core 1"
                                                                             Flash Mode: "QIO 80MHz"
                                                                             Flash Size: "16MB (128Mb)"
                                                                             JTAG Adapter: "Disabled"
                                                                             Arduino Runs On: "Core 1"
                                                                             USB Firmware MSC On Boot: "Disabled"
                                                                             Partition Scheme: "16M Flash (2MB APP/12.5MB FATFS)"
                                                                             PSRAM: "OPI PSRAM"
                                                                             Upload Mode: "UARTO / Hardware CDC"
                                                                             Upload Speed: "921600"
                                                                             USB Mode: "Hardware CDC and JTAG"
                                                                             Zigbee Mode: "Disabled"

*/
//Удалить эту строчку если нашли её (для теста)
#include <SPI.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"  // Датчик углекислого газа SCD-30
#include <GxEPD2_3C.h>                       //Дисплей E-ink
#include <U8g2_for_Adafruit_GFX.h>           // Шрифты https://github.com/olikraus/U8g2_for_Adafruit_GFX
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>   // Датчик BME280 влажность,давление,температура
#include <Adafruit_TSL2591.h>  // Датчик TSL2591 освещённость
#include "RTClib.h"            // Модуль DS3231 часы реального времени
#include <math.h>              // для isnan()


// Настройки пинов I2C (если не стандартные)
#define SDA_PIN 8
#define SCL_PIN 9
// Настройки
#define SCREEN_WIDTH 296
#define SCREEN_HEIGHT 128
// Настройки пинов SPI для E-inc
static const uint8_t EPD_BUSY = 17;  // to EPD BUSY
static const uint8_t EPD_CS = 13;    // to EPD CS
static const uint8_t EPD_RST = 11;   // to EPD RST
static const uint8_t EPD_DC = 12;    // to EPD DC

static const uint8_t EPD_SCK = 18;   // to EPD CLK
static const uint8_t EPD_MISO = -1;  // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 10;  // to EPD DIN / SDA
/*
Nr name  color   ESP32-S3 pin
1  BUSY  violet  17 
2  RES   orange  11
3  D/C   white   12
4  CS    blue    13
5  SCL   green   18
6  SDA   yellow  10 ('SDA' terminal is known as 'MOSI')
7  GND   black   GND
8  VCC   red     VCC is 3.3 volt, not 5 volt !
*/

// Создаем объекты датчиков
SCD30 airSensor;                                                                                                  // Инициализация SCD-30
Adafruit_BME280 bme;                                                                                              // Инициализация BME280
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);                                                                    // Инициализация TSL2591
RTC_DS3231 rtc;                                                                                                   // Инициализация DS3231
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));  // GDEM029C90 128x296, SSD1680
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;                                                                                  // Select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

// Переменные хранящие значения с модулей
byte batt = 100; // %
// DS3231
int year;
byte month;
byte day;
byte hour;
byte minute;
byte second;
// SCD30
unsigned int CO2;  // (ppm)
float Temp_SCD30;  // (°C)
float RH_SCD30;    // (%)
// BME280
float Press;        // (hPa)
float Temp_BME280;  // (°C)
float RH_BME280;    // (%)
float Alt;          // (m)
// TSL2591
float lux;  // (Lux)
//Минимальное и максимальное значение за сутки 
 byte day_mm = 100;
unsigned int CO2_min;// (ppm)
unsigned int CO2_max;// (ppm)
float Temp_BME280_min;  // (°C)
float Temp_BME280_max;  // (°C)
float RH_BME280_min;    // (%)
float RH_BME280_max;    // (%)
//Для графика 
int disp_T = 0; 
int Temp_Data[104];
//переменные для работы
unsigned long tm1 = 18000;



void loop() {

  if (tm1 <= millis()) {
    tm1 = millis() + 121000;
    Read_sensors();
    Disp1_refresh();

    Serial.println("------------------------------------------------------------");
    Serial.print("Временная метка - ");
    Serial.print(year);
    Serial.print(".");
    Serial.print(month);
    Serial.print(".");
    Serial.print(day);
    Serial.print(" , ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
    Serial.println("Показания с датчика SCD30");
    Serial.print(Temp_SCD30);
    Serial.print(" (°C)  ");
    Serial.print(RH_SCD30);
    Serial.print(" (%)  ");
    Serial.print(CO2);
    Serial.println(" (ppm)  ");
    Serial.println("Показания с датчика BME280");
    Serial.print(Temp_BME280);
    Serial.print(" (°C)  ");
    Serial.print(RH_BME280);
    Serial.print(" (%)  ");
    Serial.print(Press);
    Serial.print(" (hPa)  ");
    Serial.print(Alt);
    Serial.println(" (m)  ");
    Serial.println("Показания с датчика TSL2591");
    Serial.print(lux);
    Serial.println(" (lux)  ");
    Serial.println("------------------------------------------------------------");
  }
}
