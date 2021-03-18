#pragma once
#include "include.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "dash.h"
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


const char* sword_toogle(){
    static char status_otkljucan[]  {"Otkljucan"};
    static char status_zakljucan[]  {"Zakljucan"};
    if (all_pass) { // if true -- unlock
        Serial.println("Lock");
        all_pass=false;
        return status_zakljucan;
    }
    else { // all pass is FALSE -- lock initiated
        Serial.println("Unlock");
        all_pass = true;
        return status_otkljucan;
    }
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