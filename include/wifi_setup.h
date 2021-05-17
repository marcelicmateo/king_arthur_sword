#pragma once
#include "include.h"
#include <ESP8266WiFi.h>

// #define DEV

constexpr char ssid[] = "Kralj";
constexpr char password[] = "artur123";
constexpr int channel = 1;
constexpr int ssid_hidden = 0; // hidden if true
constexpr int max_connection = 4;

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 100);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);

void wifi_setup(void) {
#ifdef DEV_WIFI
  WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
#else
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
    ESP.reset();
  }
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