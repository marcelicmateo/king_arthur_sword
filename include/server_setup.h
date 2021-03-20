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

const char* ir_pir_state(){
  String state_true{"TRUE"};
  String state_false{"FALSE"};
  static String c;
  static bool ir{0}, pir{0};
  ir = !(bool)digitalRead(INFRARED_OBSTICLE_SENSOR);
  pir = (bool)digitalRead(PIR_SENSOR);
  c = (String)(ir ? state_true:state_false) + (String)(";") + (String)(pir ? state_true:state_false);
  return c.c_str();

}

String processor(const String& var){
  //Serial.println(var);
  if(var == "STATUS_MAC"){
    return (all_pass ? (String)("Otkljucan") : (String)("Zakljucan"));
  }
  else if(var == "STATUS_MAC_GUMB"){
return (all_pass ? (String)("Zakljucaj") : (String)("Otkljucaj"));
  }else if (var == "PIR_SENSOR"){
    return (String)(digitalRead(PIR_SENSOR) ? "TRUE" : "FALSE");
  }else if (var == "IR_SENSOR"){
    return (String)(digitalRead(INFRARED_OBSTICLE_SENSOR) ? "TRUE" : "FALSE");
  }
  return String();
}


void server_setup(){
     // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", dash, processor);
  });
 

  server.on("/sword_toogle", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Sword-Toogle");
    request->send_P(200, "text/plain", sword_toogle());
  });
  server.on("/ir_pir_sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: IR and Pir sensor");
    request->send_P(200, "text/plain", ir_pir_state());
  });
 

  // Start server
  server.begin();
}