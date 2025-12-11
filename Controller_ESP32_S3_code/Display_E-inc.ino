
#include "Display_image.h"

void InitialiseDisplay() {
  display.init(115200, true, 2, false);
  SPI.end();
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
  display.setRotation(1);                     // Use 1 or 3 for landscape modes
  u8g2Fonts.begin(display);                   // connect u8g2 procedures to Adafruit GFX
  u8g2Fonts.setFontMode(1);                   // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);              // left to right (this is default)
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);  // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);  // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_helvB10_tf);    // Explore u8g2 fonts from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
}

void drawString(int x, int y, String text) {
  int16_t x1, y1;  //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  display.setTextWrap(false);
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  u8g2Fonts.setCursor(x, y + h);
  u8g2Fonts.print(text);
}

void start_image_E_inc() {
  u8g2Fonts.setFont(u8g2_font_helvB14_tf);
  uint8_t distance = 20;
  uint8_t counter = 0;
  const uint8_t startX = 10;
  const uint8_t startY = 10;
  const char *DISPLAY_TYPE = "Type: WeAct Epaper Module";
  const char *DISPLAY_INTERFACE = "Interface: SPI | Chip: SSD1680";
  const char *DISPLAY_DRIVER = "Driver: GxEPD2_290_C90c";
  const char *DISPLAY_SIZE = "Size: 2.9-inch 128 x 296 pixels";
  const char *DISPLAY_COLORS = "Colors: Red, Black and White";
  const char *PROGRAM_AUTHOR = "Authors: Maxim S. and Vika Z.";

  drawString(startX, startY + 0 * distance, DISPLAY_TYPE);
  drawString(startX, startY + 1 * distance, DISPLAY_INTERFACE);
  drawString(startX, startY + 2 * distance, DISPLAY_DRIVER);
  drawString(startX, startY + 3 * distance, DISPLAY_SIZE);
  u8g2Fonts.setForegroundColor(GxEPD_RED);
  drawString(startX, startY + 4 * distance, DISPLAY_COLORS);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(startX, startY + 5 * distance, PROGRAM_AUTHOR);
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GxEPD_RED);

  display.display(false);  // Full screen update mode

  // Important use bring the display in one of the two modes to
  // avoid a damage of the display that cannot get repaired.
  //display.powerOff();
  display.hibernate();
}

void Disp1_refresh() {

  display.fillScreen(GxEPD_WHITE);

  // Вывод на дисплей показателей СО2 ---------
  u8g2Fonts.setFont(u8g2_font_inr33_mn);  // 33px только цифры
  if (CO2 >= 1400) {
    u8g2Fonts.setForegroundColor(GxEPD_RED);
  } else {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  }
  int spix = 0;
  char out[10];
  dtostrf(CO2, 5, 0, out);
  if (CO2 < 1000) {
    for (int i = 0; i < 10 - 1; i++) {
      out[i] = out[i + 1];
    }
    spix = spix + 33;
  }
  if (CO2 < 10000) {
    for (int i = 0; i < 10 - 1; i++) {
      out[i] = out[i + 1];
    }
    spix = spix + 33;
  }
  drawString(0, 27, out);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setFont(u8g2_font_crox4h_tf);  // 14px
  drawString(145 - spix, 23, "ppm");
  u8g2Fonts.setForegroundColor(GxEPD_RED);
  drawString(145 - spix, 9, "CO");
  u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
  drawString(172 - spix, 9, "2");
  display.drawRect(0, 0, 187 - spix, 38, GxEPD_RED);
  //----------------------------------------------------

  //Вывод на дисплей показателей температуры
  u8g2Fonts.setFont(u8g2_font_inr21_mn);  // 21px только цифры
  if (Temp_BME280 >= 28.0 || Temp_BME280 <= 21.0) {
    u8g2Fonts.setForegroundColor(GxEPD_RED);
  } else {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  }
  int spix1 = 0;
  char out1[10];
  char out2[4];
  char out3[2];
  dtostrf(Temp_BME280, 5, 1, out1);
  if (Temp_BME280 >= 0.0) {
    spix1 = spix1 + 18;
  }
  for (int i = 0; i < 4; i++) {
    if (out1[i] == '.') {
      break;
    }
    out2[i] = out1[i];
  }
  out2[3] = '\0';
  out3[0] = out1[4];
  out3[1] = '\0';
  drawString(61 - 10 - spix1, 56, ".");
  drawString(0 - spix1, 56, out2);
  drawString(84 - 20 - spix1, 56, out3);
  u8g2Fonts.setFont(u8g2_font_crox4h_tf);  // 14px
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(103 - 20 - spix1, 56, "°C");
  if (Temp_BME280 >= 28.0) {
    display.drawBitmap(103 + 28 - 20 - spix1, 41, fire2, fire2_widht, fire2_height, GxEPD_RED);
    display.drawBitmap(103 + 28 - 20 - spix1, 41, fire1, fire1_widht, fire1_height, GxEPD_BLACK);
  }
  if (Temp_BME280 <= 21.0) {
    display.drawBitmap(103 + 28 - 20 - spix1, 41, snowflake1, snowflake1_widht, snowflake1_height, GxEPD_BLACK);
  }
  if (Temp_BME280 < 28.0 && Temp_BME280 > 21.0) {
    display.drawBitmap(103 + 28 - 20 - spix1, 41, sheet1, sheet1_widht, sheet1_height, GxEPD_BLACK);
  }
  display.drawRect(0, 39, 138 - spix1, 29, GxEPD_RED);
  //--------------------------------------

  //Вывод на дисплей показателей влажности
  u8g2Fonts.setFont(u8g2_font_inr21_mn);  // 21px только цифры
  if (RH_BME280 >= 75.0 || RH_BME280 <= 40.0) {
    u8g2Fonts.setForegroundColor(GxEPD_RED);
  } else {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  }
  int spix2 = 0;
  char out4[10];
  char out5[4];
  char out6[2];
  dtostrf(RH_BME280, 5, 1, out4);
  if (RH_BME280 < 100.0) {
    spix2 = spix2 + 18;
  }
  for (int i = 0; i < 4; i++) {
    if (out4[i] == '.') {
      break;
    }
    out5[i] = out4[i];
  }
  out5[3] = '\0';
  out6[0] = out4[4];
  out6[1] = '\0';
  drawString(61 - 10 - spix2, 86, ".");
  drawString(0 - spix2, 86, out5);
  drawString(84 - 20 - spix2, 86, out6);
  u8g2Fonts.setFont(u8g2_font_crox4h_tf);  // 14px
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(103 - 20 - spix2, 87, "%");
  u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
  u8g2Fonts.setForegroundColor(GxEPD_RED);
  drawString(103 - 20 - spix2, 72, "Rh");

  if (RH_BME280 >= 75.0) {
    display.drawBitmap(103 + 28 - 20 - spix2, 71, drops1, drops1_widht, drops1_height, GxEPD_BLACK);
  }
  if (RH_BME280 <= 40.0) {
    display.drawBitmap(103 + 28 - 20 - spix2, 71, cactus, cactus_widht, cactus_height, GxEPD_BLACK);
    display.drawBitmap(103 + 28 - 20 - spix2, 71, cactus_flow, cactus_flow_widht, cactus_flow_height, GxEPD_RED);
  }
  if (RH_BME280 > 40.0 && RH_BME280 < 75.0) {
    display.drawBitmap(103 + 28 - 20 - spix2, 71, sheet1, sheet1_widht, sheet1_height, GxEPD_BLACK);
  }

  display.drawRect(0, 69, 138 - spix2, 29, GxEPD_RED);
  //--------------------------------------

  //Вывод на дисплей показателей давления и высоты
  u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(2, 104, "Pres:");
  drawString(4, 116, "  Alt:");
  char out7[10];
  char out8[10];
  dtostrf(Press, 6, 1, out7);
  dtostrf(Alt, 6, 1, out8);
  drawString(30, 104, out7);
  drawString(30, 116, out8);
  drawString(71, 104, "hPa");
  drawString(71, 116, "m");
  dtostrf(Press * 0.750063755, 3, 0, out7);
  drawString(94, 104, ",");
  drawString(98, 104, out7);
  drawString(120, 104, "mmHg");
  display.drawRect(0, 99, 157, 29, GxEPD_RED);
  //--------------------------------------

  //Верхний хот бар
  u8g2Fonts.setFont(u8g2_font_battery19_tn);  // 19 px battery
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);

  if (batt >= 90) {
    drawString(286, 13, "5");
  }
  if (batt < 90 && batt >= 80) {
    drawString(286, 13, "4");
  }
  if (batt < 80 && batt >= 60) {
    drawString(286, 13, "3");
  }
  if (batt < 60 && batt >= 40) {
    drawString(286, 13, "2");
  }
  if (batt < 40 && batt >= 20) {
    drawString(286, 13, "1");
  }
  if (batt < 20 && batt >= 0) {
    drawString(286, 13, "0");
  }
  u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9 px battery
  char out9[4];
  dtostrf(batt, 3, 0, out9);
  u8g2Fonts.setFontDirection(1);
  drawString(275, -10, out9);
  u8g2Fonts.setFontDirection(0);

  display.drawBitmap(260, 2, wi_fi_100small, wi_fi_100small_widht, wi_fi_100small_height, GxEPD_BLACK);  //вайфай
  u8g2Fonts.setFont(u8g2_font_crox4h_tf);                                                                // 14 px                                               // 21px
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(209, 10, ":");
  drawString(234, 10, ":");
  char out10[2];
  dtostrf(hour, 2, 0, out10);
  if (hour < 10) {
    out10[0] = '0';
  }
  drawString(189, 11, out10);

  dtostrf(minute, 2, 0, out10);
  if (minute < 10) {
    out10[0] = '0';
  }
  drawString(214, 11, out10);

  dtostrf(second, 2, 0, out10);
  if (second < 10) {
    out10[0] = '0';
  }
  drawString(239, 11, out10);

  u8g2Fonts.setFont(u8g2_font_crox3tb_tf);  // 11 px                                               // 21px
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(231, 26, ":");
  drawString(254, 26, ":");
  dtostrf(day, 2, 0, out10);
  if (day < 10) {
    out10[0] = '0';
  }
  drawString(212, 27, out10);

  dtostrf(month, 2, 0, out10);
  if (month < 10) {
    out10[0] = '0';
  }
  drawString(235, 27, out10);
  char out11[4];
  dtostrf(year, 4, 0, out11);
  drawString(258, 27, out11);

  display.drawRect(187, 0, 109, 38, GxEPD_RED);
  //--------------------------------------

  // Показатель освещённости
  u8g2Fonts.setFont(u8g2_font_crox4h_tf);  // 14 px
  if (lux <= 200) {
    u8g2Fonts.setForegroundColor(GxEPD_RED);
  } else {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  }
  char out12[7];
  if (lux < 100) {
    dtostrf(lux, 7, 2, out12);
    drawString(200, 56, out12);
  } else {
    dtostrf(lux, 7, 1, out12);
    drawString(189, 56, out12);
  }
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(258, 56, "Lux");
  display.drawRect(187, 39, 109, 29, GxEPD_RED);
  //--------------------------------------

  // Мин и макс значения и график через раз
  int x0 = 158;
  int y0 = 69;
  if (disp_T == 0) {
    disp_T = 1;

    u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    drawString(x0 + 2, y0 + 20, "max:");
    drawString(x0 + 2, y0 + 35, "min:");
    drawString(x0 + 30, y0 + 5, "ppm");
    drawString(x0 + 73, y0 + 5, "°C");
    drawString(x0 + 110, y0 + 5, "%");
    char out13[10];
    dtostrf(CO2_min, 5, 0, out13);
    drawString(x0 + 30, y0 + 35, out13);
    dtostrf(CO2_max, 5, 0, out13);
    drawString(x0 + 30, y0 + 20, out13);
    dtostrf(Temp_BME280_min, 5, 1, out13);
    drawString(x0 + 73, y0 + 35, out13);
    dtostrf(Temp_BME280_max, 5, 1, out13);
    drawString(x0 + 73, y0 + 20, out13);
    dtostrf(RH_BME280_min, 4, 1, out13);
    drawString(x0 + 110, y0 + 35, out13);
    dtostrf(RH_BME280_max, 4, 1, out13);
    drawString(x0 + 110, y0 + 20, out13);
  } else {
    disp_T = 0;

    u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    drawString(x0 + 2, y0 + 20, "max:");
    drawString(x0 + 2, y0 + 35, "min:");
    drawString(x0 + 30, y0 + 5, "ppm");
    drawString(x0 + 73, y0 + 5, "°C");
    drawString(x0 + 110, y0 + 5, "%");
    char out13[10];
    dtostrf(CO2_min, 5, 0, out13);
    drawString(x0 + 30, y0 + 35, out13);
    dtostrf(CO2_max, 5, 0, out13);
    drawString(x0 + 30, y0 + 20, out13);
    dtostrf(Temp_BME280_min, 5, 1, out13);
    drawString(x0 + 73, y0 + 35, out13);
    dtostrf(Temp_BME280_max, 5, 1, out13);
    drawString(x0 + 73, y0 + 20, out13);
    dtostrf(RH_BME280_min, 4, 1, out13);
    drawString(x0 + 110, y0 + 35, out13);
    dtostrf(RH_BME280_max, 4, 1, out13);
    drawString(x0 + 110, y0 + 20, out13);
    // u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
    // u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    // drawString(x0 + 2, y0 + 3, "ppm");
    // drawString(x0 + 110, y0 + 49, "hour");
    // u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
    // u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    // drawString(x0 + 2, y0 + 15, "6000");
    // drawString(x0 + 2, y0 + 30, "3200");
    // drawString(x0 + 2, y0 + 45, "400");
    // u8g2Fonts.setFont(u8g2_font_crox1hb_tf);  // 9px
    // u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    // drawString(x0 + 25, y0 + 49, "1");
    // drawString(x0 + 55, y0 + 49, "3");
    // drawString(x0 + 95, y0 + 49, "6");
    // for (int i; i < 104; i++) {
    //   display.point(x0 + 30 + i, y0 + 15 + 32 - map(Temp_Data[i], 300, 6000, y0 + 15 + 32, y0 + 15), GxEPD_BLACK)
    // }

    // display.drawRect(x0 + 30, y0 + 15, 106, 32, GxEPD_BLACK);
  }


  display.drawRect(x0, y0, 138, 59, GxEPD_RED);
  //------------------------------
  // Полный рефреш для RBW
  display.display(false);
  // Усыпление
  display.hibernate();
}
