#pragma once

#include "include.h"

void error_motor_stuck() {
  // critical error
  // motor is stuck, system halt
  state = 255;
}
void error_motor_cant_lock() {
  // error on locking sword
  state = 254;
  return;
}