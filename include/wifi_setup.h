#pragma once
#include "include.h"
#include <ESP8266WiFi.h>

// #define DEV

constexpr char ssid[] = "Kralj";
constexpr char password[] = "artur123";
constexpr int channel = 1;
constexpr int ssid_hidden = 0; // hidden if true
constexpr int max_connection = 4;

void wifi_setup(void) {
#ifndef DEV
  WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
#else
  WiFi.begin("Nema Veze", "Bodulo94");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
#endif

  return;
}