

void Read_sensors() {
  Read_SCD30();
  Read_BME280();
  Read_TSL2591();
  Read_DS3231();
  if (dbS["day_mm"] != day && CO2 != 0) {
    dbS["day_mm"] = day;
    dbS["CO2_min"] = CO2;
    dbS["CO2_max"] = CO2;
    dbS["Temp_BME280_min"] = Temp_BME280;
    dbS["Temp_BME280_max"] = Temp_BME280;
    dbS["RH_BME280_min"] = RH_BME280;
    dbS["RH_BME280_max"] = RH_BME280;
  }


  if (dbS["CO2_min"] > CO2 && CO2 != 0 ) {
    dbS["CO2_min"] = CO2;
  }
  if (dbS["Temp_BME280_min"] > Temp_BME280) {
    dbS["Temp_BME280_min"] = Temp_BME280;
  }
  if (dbS["RH_BME280_min"] > RH_BME280) {
    dbS["RH_BME280_min"] = RH_BME280;
  }
  if (dbS["CO2_max"] < CO2 && CO2 != 0) {
    dbS["CO2_max"] = CO2;
  }
  if (dbS["Temp_BME280_max"] < Temp_BME280) {
    dbS["Temp_BME280_max"] = Temp_BME280;
  }
  if (dbS["RH_BME280_max"] < RH_BME280) {
    dbS["RH_BME280_max"] = RH_BME280;
  }

  if (CO2 != 0) {
    dbS["CO2db"] = CO2;
  }
  dbS.update();
}

void Read_DS3231() {  // Чтение показаний с модуля часов реального времени
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
}

void Read_SCD30() {  // Чтение показаний с датчика углекислого газа
  if (airSensor.dataAvailable()) {
    CO2 = airSensor.getCO2();
    Temp_SCD30 = airSensor.getTemperature();
    RH_SCD30 = airSensor.getHumidity();
  }
}

void Read_BME280() {  // Чтение показаний с датчика давления влажности и температуры
  Press = bme.readPressure() / 100.0;
  Temp_BME280 = bme.readTemperature();
  RH_BME280 = bme.readHumidity();
  Alt = bme.readAltitude(1013.25);  // 1013.25 — стандартное давление на уровне моря
}

void Read_TSL2591() {  // Чтение показаний с датчика освещённости
  float lux1;
  tsl.setGain(TSL2591_GAIN_LOW);                 // чувствительность = 1
  tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // Время интеграции 600 мс
  sensors_event_t event;
  tsl.getEvent(&event);
  lux1 = event.light;
  // Подстройка чувствительности
  int count1 = 0;
  int count2 = 0;
  while ((lux1 == 0 || lux1 > 10000000.0 || isnan(lux1) || ((lux1 < 3 && lux1 >= 0) && count1 < 1)) && count1 < 2) {
    count1++;
    switch (count1) {
      case 1:
        tsl.setGain(TSL2591_GAIN_MED);
        //Serial.println("Усиление = 25 ");
        break;
      case 2:
        tsl.setGain(TSL2591_GAIN_HIGH);
        //Serial.println("Усиление = 428 ");
        break;
    }
    tsl.getEvent(&event);
    lux1 = event.light;
  }
  // Подстройка времени интегрирования
  if (lux1 == -1 && count2 < 1) {
    tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
    //Serial.println("Время инт = 200 ");
    tsl.getEvent(&event);
    lux1 = event.light;
  }
  //Крайние состояния показаний
  if (isnan(lux1)) {
    lux1 = 0;
  }
  if (lux1 > 100000 || lux1 < 0) {
    lux1 = 100000;
  }
  lux = lux1;
}
