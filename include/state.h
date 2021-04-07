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

void play_bingo(){};