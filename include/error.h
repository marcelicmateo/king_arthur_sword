#pragma once

#include "include.h"

void error_motor_stuck() {
  // critical error
  // motor is stuck, system halt
  flag_error_stuck = true;
  state = 255;
}
void error_motor_cant_lock() {
  // error on locking sword
  flag_error_lock = true;
  state = 254;
  return;
}