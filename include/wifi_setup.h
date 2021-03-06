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
IPAddress local_IP(192, 168, 4, 1);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void wifi_setup(void) {

  WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  return;
}