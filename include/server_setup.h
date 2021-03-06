#pragma once

#include "dash.h"
#include "include.h"

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
      (String)unlock_flag + ';' + (String)currentPlayer;
  return c.c_str();
}

const char *generate_new_bingo(AsyncWebServerRequest *request) {
  static String buff{"Generating"};
  numberOfContestants = request->getParam(0)->value().toInt();
  numberOfWinnings = request->getParam(1)->value().toInt();
  Serial.println("N of Contestans: " + (String)numberOfContestants);
  Serial.println("N of Winners: " + (String)numberOfWinnings);
  f_generate_new_bingo = 1; // set flag to generate new bingo;
  return buff.c_str();
}

uint8_t state_to_number(const uint8_t s) {
  uint8_t ret = 0;
  switch ((s & 0b11111000)) {
  // clean cases
  case 0b10000000:
    ret = 1;
    break;
  case 0b01000000:
    ret = 2;
    break;
  case 0b00100000:
    ret = 3;
    break;
  case 0b00010000:
    ret = 4;
    break;
  default:
    break;
  }
  return ret;
}

uint8_t *change_state(AsyncWebServerRequest *request) {
  Serial.println("State value: " + request->getParam(0)->value());
  long shift = request->getParam(0)->value().toInt();
  state =
      ((state & 0b00000111) | (1 << (8 - shift))); // clear states and set new
  save_state(state);
  static uint8_t ret{0};
  ret = state_to_number(state);
  return &ret;
}

uint8_t *exit_error() {
  system_restore_state();
  static uint8_t ret;
  ret = state_to_number(state);
  return &ret;
}

const char *system_status() { return system_buffer.c_str(); }

String winning_numbers() {
  String buf{""};
  int i{0};
  for (i = 0; i < numberOfWinnings - 1; i++) {
    buf += (String)winningNumbers[i] + ",";
    yield();
  }
  buf += winningNumbers[i];
  return buf;
}

const char *change_probability(AsyncWebServerRequest *request) {
  static String buf{};
  probability_for_king = request->getParam(0)->value().toInt();
  buf = (String)probability_for_king;
  Serial.println("New chance KING: " + buf);
  return buf.c_str();
}

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
  } else if (var == "NACIN_RADA") {
    return (String)state_to_number(state);
  } else if (var == "SYSTEM_STATUS") {
    return ((String)0);
  } else if (var == "NUMBER_OF_PLAYERS") {
    return (String)numberOfContestants;
  } else if (var == "NUMBER_OF_WINNERS")
    return (String)numberOfWinnings;
  else if (var == "CURRENT_PLAYER")
    return (String)currentPlayer;
  else if (var == "OLD_CHANCE")
    return (String)probability_for_king;
  else if (var == "WINNING_NUMBERS")
    return String();
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
  // New BINGO
  server.on("/generate_new_bingo", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              Serial.println("GET: New BINGO");
              request->send_P(200, "text/plain", generate_new_bingo(request));
            });
  // change working mode of program logic
  server.on("/change_state", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: Change state");
    request->send_P(200, "text/plain", change_state(request), 1);
  });

  server.on("/system_status", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: system_status");
    request->send_P(200, "text/plain", system_status());
  });

  server.on("/exit_error", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("GET: exit_error");
    request->send_P(200, "text/plain", exit_error(), 1);
  });

  server.on("/change_probability", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              Serial.println("GET: change_probability");
              request->send_P(200, "text/plain", change_probability(request));
            });

  // Start server
  server.begin();
}