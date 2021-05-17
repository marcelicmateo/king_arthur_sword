#pragma once

#include "include.h"

void error_motor_stuck() {
  // critical error
  // motor is stuck, system halt
  save_state(0b00000001);
}

void error_motor_cant_lock() {
  // error on locking sword

  save_state(0b00000010);
  return;
}

bool system_restore_state() {
  Serial.println("Restoring system -- " + (String)state);
  state &= 0b11111100; // remove flags of errors
  save_state(state);
  return true;
}
