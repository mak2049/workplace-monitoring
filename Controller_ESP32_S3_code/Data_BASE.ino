GyverDBFile dbS(&LittleFS, "/data_SENSORS.db");  //База данных для показаний с датчиков


DB_KEYS(
  //Минимальное и максимальное значение за сутки
  KS,
  CO2db,            // (ppm)
  day_mm,           //
  CO2_min,          // (ppm)
  CO2_max,          // (ppm)
  Temp_BME280_min,  // (°C)
  Temp_BME280_max,  // (°C)
  RH_BME280_min,    // (%)
  RH_BME280_max);   // (%)



void BD_TIC(void* pvParameters) {
  dbS[KS::CO2db] = 0;
  dbS[KS::day_mm] = 100;
  dbS[KS::CO2_min] = 0;
  dbS[KS::CO2_max] = 0;
  dbS[KS::Temp_BME280_min] = 0.0;
  dbS[KS::Temp_BME280_max] = 0.0;
  dbS[KS::RH_BME280_min] = 0.0;
  dbS[KS::RH_BME280_max] = 0.0;

  dbS.begin();  // прочитать данные из файла
  CO2 = dbS["CO2db"];
  while (1) {
    dbS.tick();
    vTaskDelay(100);
  }
}