#pragma once

#include "include.h"
uint8 previousState{0}; // save previous state

void error_motor_stuck() {
  // critical error
  // motor is stuck, system halt
  previousState = state;
  state = 127;
}

void error_motor_cant_lock() {
  // error on locking sword
  previousState = state;
  state = 126;
  return;
}

bool system_restore_state() {
  Serial.println("Restoring system -- " + (String)state);
  switch (state) {
  case 126:
    state = previousState;
    previousState = 0;
    Serial.println("System restored -- " + (String)state);
    return (true);
    break;
  case 127:
    state = previousState;
    previousState = 0;
    Serial.println("System restored -- " + (String)state);
    return (true);
  default:
    break;
  }
  return false;
}
