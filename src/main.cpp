#include "bingo.h"
#include "error.h"
#include "include.h"
#include "pin.h"
#include "server_setup.h"
#include "state.h"
#include "sword.h"
#include "wifi_setup.h"

#define Serial                                                                 \
  if (SERIAL)                                                                  \
  Serial

void setup() {

  Serial.begin(9600);
  delay(500); // get serial up

  Serial.println("SETUP BEGIN");
  init_gpio_pins();
  motor.attach(STEPER_MOTOR);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    ESP.reset();
  }

  get_bingo_data();
  load_state();
  load_hold();
  load_flags();

  wifi_setup();

  server_setup();

  if (!lock_flag && !unlock_flag) {
    unlock_sword();
  }

  Serial.println(system_get_cpu_freq());
  Serial.println("Begin loop");
  Serial.println(state, BIN);
}

unsigned long current_milis = millis();
unsigned long previous_milis = 0;

void handle_flag_cases(uint8_t f) {
  switch (f) {
  case 0b00000010:
    error_motor_cant_lock();
    break;
  case 0b00000001:
    error_motor_stuck();
    break;
  default:
    break;
  }
}

void generate_system_status(String *buf) {
  static String s_error;
  static String n_players;
  static String n_winners;
  static String bingo_numbers;
  static String n_curent_pl;
  // static String s_state;
  lock = 1;
  s_error = (String)(state & 0b0000111);
  n_players = (String)numberOfContestants;
  n_winners = (String)numberOfWinnings;
  bingo_numbers = winning_numbers();
  yield();
  n_curent_pl = (String)currentPlayer;

  *buf = s_error + ';' + n_curent_pl + ';' + n_players + ';' + n_winners + ';' +
         bingo_numbers;
  lock = 0;
  yield();
  Serial.println("Buffer: " + *buf);
  return;
}

void loop() {
  // static uint8_t flag_case;

  // AsyncElegantOTA.loop(); // service OTA updates
  /*
    flag_case = state & 0b0000111; // mask flag cases
    handle_flag_cases(flag_case);
  */

  switch (state) { // state machine
  // clean cases
  case 0b10000000:
    sword_prema_unlocked();
    break;
  case 0b01000000:
    sword_prema_locked();
    break;
  case 0b00100000:
    play_bingo();
    break;
  case 0b00010000:
    play_random();
    break;
  default:
    // 0
    // should never come to this value
    // wdt reset hapend
    Serial.print("Error, state = ");
    Serial.println(state, BIN);
  }

  generate_system_status(&system_buffer);

  // yield(); // let ESP do stuff
  delay(50);
}