#pragma once

#include "include.h"

void init_gpio_pins() {
  pinMode(PIR_SENSOR, INPUT);
  pinMode(INFRARED_OBSTICLE_SENSOR, INPUT);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, INPUT);
  return;
}