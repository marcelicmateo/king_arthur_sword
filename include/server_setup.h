#pragma once

#include "dash.h"
#include "include.h"

#include <AsyncElegantOTA.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

const char *ir_pir_state() {
  static String c;
  static bool ir{0}, pir{0};
  ir = is_sword_present();
  pir = presistane_flag;
  c = (String)ir + ";" + (String)pir + ";" + (String)lock_flag + ";" +
      (String)unlock_flag;
  return c.c_str();
}

const char *change_probability(AsyncWebServerRequest *request) {
  probability_to_pass = request->getParam(0)->value().toInt();
  Serial.println("New value: " + (String)probability_to_pass);
  return request->getParam(0)->value().c_str();
}

const char *change_state(AsyncWebServerRequest *request) {
  state = request->getParam(0)->value().toInt();
  Serial.println("New value: " + (String)state);
  return request->getParam(0)->value().c_str();
}

uint8_t *system_status() { return &state; }

String processor(const String &var) {
  // replace place holder values in HTML page
  if (var == "STATUS_MAC") {
    return (all_pass ? (String)("Otkljucan") : (String)("Zakljucan"));
  } else if (var == "STATUS_MAC_GUMB") {
    return (all_pass ? (String)("Zakljucaj") : (String)("Otkljucaj"));
  } else if (var == "PIR_SENSOR") {
    return (String)(digitalRead(PIR_SENSOR) ? "TRUE" : "FALSE");
  } else if (var == "IR_SENSOR") {
    return (String)(digitalRead(INFRARED_OBSTICLE_SENSOR) ? "TRUE" : "FALSE");
  } else if (var == "POSTOTAK") {
    return (String)probability_to_pass;
  } else if (var == "NACIN_RADA") {
    return (String)state;
  } else if (var == "SYSTEM_STATUS") {
    return ((String)0);
  }
  return String();
}

void server_setup() {
  // root page, main dashboard
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dash, processor);
  });
  // GET ir and pir sensor readings, interval of .5s
  server.on("/ir_pir_sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: IR and Pir sensor");
    request->send_P(200, "text/plain", ir_pir_state());
  });
  // change probability of unlocking a sword
  server.on("/change_probability", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              Serial.println("GET: Change Probability");
              request->send_P(200, "text/plain", change_probability(request));
            });
  // change working mode of program logic
  server.on("/change_state", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: Change WORK");
    request->send_P(200, "text/plain", change_state(request));
  });

  server.on("/system_status", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: system_status");
    request->send_P(200, "text/plain", system_status(), 1);
  });

  // OTA update server
  AsyncElegantOTA.begin(&server);
  // Start server
  server.begin();
}