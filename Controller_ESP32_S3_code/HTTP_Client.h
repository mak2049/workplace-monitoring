#pragma once
#define GHTTP_CLIENT
#include <GyverHTTP.h>

void sendTelemetry(float t, float h, float p, int co2, float lux) {
  if (WiFi.status() != WL_CONNECTED) return;

  ghttp.begin("http://192.168.1.100:8080/api/v1/telemetry");

  ghttp.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"device\":\"ws-01\",";
  json += "\"t\":" + String(t, 1) + ",";
  json += "\"h\":" + String(h, 1) + ",";
  json += "\"p\":" + String(p, 1) + ",";
  json += "\"co2\":" + String(co2) + ",";
  json += "\"lux\":" + String(lux, 0);
  json += "}";

  int code = ghttp.POST(json);

  if (code == 200) {
    Serial.println("HTTP: OK");
  } else {
    Serial.print("HTTP error: ");
    Serial.println(code);
  }

  ghttp.end();
}


void httpTask(void*){
  for(;;){
    // собрать свежие измерения (из общих переменных/очереди)
    sendTelemetry(Temp_BME280, RH_BME280, Press, CO2, lux);
    vTaskDelay(pdMS_TO_TICKS(30000));
  }
}