#include "bingo.h"
#include "error.h"
#include "include.h"
#include "pin.h"
#include "server_setup.h"
#include "state.h"
#include "sword.h"
#include "wifi_setup.h"

#define DEBUG false
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  yield();
  delay(500); // get serial up
#endif

  Serial.println("SETUP BEGIN");
  init_gpio();
  motor.attach(STEPER_MOTOR);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  get_bingo_data();

  wifi_setup();
  server_setup();

  if (!lock_flag && !unlock_flag) {
    unlock_sword();
  }

  Serial.println("Begin loop");
}

unsigned long current_milis = millis();
unsigned long previous_milis = 0;

void loop() {

  AsyncElegantOTA.loop(); // service OTA updates

  switch (state) { // state machine
  case 127:
    // critical error, system halt
    Serial.println("Critical error, system halt");
    break;
  case 126:
    // lock error
    Serial.println("Sword cant be locked");
    break;
  case 1:
    sword_prema_unlocked();
    break;
  case 2:
    sword_prema_locked();
    break;
  case 3:
    play_bingo();
    break;
  case 4:
    play_random();
    break;
  default:
    // 0
    // should never come to this value
    // wdt reset hapend
    Serial.println("Error, reseting system");
    yield();
    ESP.restart();
  }

  yield(); // let ESP do stuff
  delay(250);
}
