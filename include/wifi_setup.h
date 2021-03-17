#pragma once

#include <ESP8266WiFi.h>


constexpr char ssid[] = "Kralj";
constexpr char password[] = "artur123";
constexpr int channel = 1;
constexpr int ssid_hidden = 0; // hidden if true
constexpr int max_connection = 4;

void wifi_setup(void){
WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
IPAddress IP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(IP);
return;
}