
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "/data.db");  //База данных инициализация
// указывается заголовок меню, подключается база данных
SettingsESP sett("WiFi config Workplase Monitor", &db);

DB_KEYS(
  kk,
  wifi_ssid,
  wifi_pass,
  apply);

void WiFI_TIC(void* pvParameters) {
  WiFi.mode(WIFI_AP_STA);
  sett.begin();
  sett.setVersion("0.07.19.12.25"); // Отображение версии прошивки
  sett.onBuild(build);
  db.begin();
  db.init(kk::wifi_ssid, "");
  db.init(kk::wifi_pass, "");
  // ======= AP =======
  WiFi.softAP("ESP32_Workplase_Monitor_AP", "23456789");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  // ======= STA =======
  // если логин задан - подключаемся
  if (db[kk::wifi_ssid].length()) {
    WiFi.begin(db[kk::wifi_ssid], db[kk::wifi_pass]);
    Serial.print("Connect STA");
    int tries = 20;
    Serial.println();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  while (1) {
    vTaskDelay(100);
    //----------WiFi------------------
    sett.tick();
    //--------------------------------
  }
}


/* ================= SETTINGS UI ================= */
void build(sets::Builder& b) {
  {
    sets::Group g(b, "WiFi");
    b.Input(kk::wifi_ssid, "SSID");
    b.Pass(kk::wifi_pass, "Password");
    if (b.Button(kk::apply, "Save & Restart")) {
      db.update();  // сохраняем БД не дожидаясь таймаута
      ESP.restart();
    }
  }
}
