#pragma once
#include "include.h"

void sword_prema_unlocked() { // state 1
  Serial.println("State1");
  unlock_sword();
  return;
}

void sword_prema_locked() {
  Serial.println("State 2");
  lock_sword();
  return;
}

bool is_player_winner() {
  if (currentPlayer > numberOfContestants) {
    return false;
  }
  for (int i = 0; i < numberOfWinnings; i++) {
    if (currentPlayer == winningNumbers[i])
      return true;
  }
  return false;
}

void play_bingo() {
  Serial.println("State 3");

  switch (pir_states()) {
  case 0b01:
    // new human on platform
    change_current_player(currentPlayer + 1);
    if (is_player_winner()) {
      unlock_sword();
    }
    break;
  case 0b11:
    // human on platform do nothing
    break;
  case 0b10:
    // human left platform
    if (is_sword_present()) {
      lock_sword();
    }
    break;
  case 0b00:
    // no one is on platform
    // do nothing
    if (is_sword_present()) {
      lock_sword();
    }
    break;
  default:
    break;
  }
}

#include <ESP8266TrueRandom.h>

bool is_player_king() {
  uint16 p{(uint16)ESP8266TrueRandom.random(99)};
  Serial.println("Random num: " + (String)p);
  if (p < probability_for_king)
    return true;
  return false;
}

void play_random() {
  Serial.println("State 4");

  switch (pir_states()) {
  case 0b01:
    // new human on platform
    change_current_player(currentPlayer + 1);
    if (is_player_king()) {
      unlock_sword();
    }
    break;
  case 0b11:
    // human on platform do nothing
    break;
  case 0b10:
    // human left platform
    if (is_sword_present()) {
      lock_sword();
    }
    break;
  case 0b00:
    // no one is on platform
    // do nothing
    if (is_sword_present()) {
      lock_sword();
    }
    break;
  default:
    break;
  }
}