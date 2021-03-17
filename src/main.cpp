#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <stdlib.h>

#include "dash.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG true
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial



void setup() {
  Serial.begin(9600);

  wifi_setup();
  server_setup();
}

int sword_present();

void loop() {

  Serial.println("Loop");
  delay(1000);
}