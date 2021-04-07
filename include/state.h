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
  switch (pir_states()) {
  case 0b01:
    // new human on platform
    currentPlayer++;
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