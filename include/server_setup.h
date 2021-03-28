#pragma once
#include "dash.h"
#include "include.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

const char *sword_toogle() {
  static char *c = (char *)&all_pass;
  all_pass = !all_pass;
  Serial.println(all_pass ? "Lock" : "Unlock");
  return c;
}

const char *ir_pir_state() {
  static String c;
  static bool ir{0}, pir{0};
  ir = !(bool)digitalRead(INFRARED_OBSTICLE_SENSOR);
  pir = (bool)digitalRead(PIR_SENSOR);
  c = (String)ir + ";" + (String)pir;
  return c.c_str();
}

const char *change_probability(AsyncWebServerRequest *request) {
  static int new_value;
  new_value = request->getParam(0)->value().toInt();
  probability_to_pass = new_value;
  Serial.println("New value: " + (String)probability_to_pass);
  return request->getParam(0)->value().c_str();
}

String processor(const String &var) {
  // Serial.println(var);
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
  }
  return String();
}

void server_setup() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dash, processor);
  });

  server.on("/sword_toogle", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: Sword-Toogle");
    request->send_P(200, "text/plain", sword_toogle());
  });
  server.on("/ir_pir_sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: IR and Pir sensor");
    request->send_P(200, "text/plain", ir_pir_state());
  });

  server.on("/change_probability", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              Serial.println("GET: Change Probability");
              request->send_P(200, "text/plain", change_probability(request));
            });

  AsyncElegantOTA.begin(&server);
  // Start server
  server.begin();
}