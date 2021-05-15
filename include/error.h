#pragma once

#include "include.h"
uint8 previousState{0}; // save previous state

void error_motor_stuck() {
  // critical error
  // motor is stuck, system halt
  // previousState = state;
  state |= 0b00000001;
}

void error_motor_cant_lock() {
  // error on locking sword
  // previousState = state;
  state |= 0b00000010;
  return;
}

bool system_restore_state() {
  Serial.println("Restoring system -- " + (String)state);
  state &= 0b11111100; // remove flags of errors
  return true;
}
