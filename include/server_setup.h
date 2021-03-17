#pragma once
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "dash.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

const char *sword_lock(){
    const char *c = "Locked";
    return c;
}

const char * sword_unlock(){
    const char *c = "Unlocked";
    return c;
}

uint8_t perma_open=0;
const char * sword_toogle(){
    static int c=0; // unlocked
    const char *status = "open" ;
    if (c == 0) {
        c=1;
        perma_open=1;
        status="Otkljucan";
    }
    else {
        c=0;
        perma_open = 0;
        status="Zakljucan";
    }
    return status;
}



void server_setup(){
     // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dash);
  });
 

  server.on("/sword_toogle", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Sword-Toogle");
    request->send_P(200, "text/plain", sword_toogle());
  });

  // Start server
  server.begin();
}