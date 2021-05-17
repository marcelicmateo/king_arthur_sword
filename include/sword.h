#pragma once
#include "include.h"

bool lock_sword() {
  bool b{0};
  // wait .5 s before locking
  delay(500);
  yield();
  b = digitalRead(PIN_IN);

  if (lock_flag == true) {
    Serial.println("Sword already locked");
    while (b == LOW) {
      motor.write(motor_stop + 2);
      yield();
      b = digitalRead(PIN_IN);
    }
    return true;
  }

  Serial.println("Locking sword");
  unsigned long pMillis = millis();
  unsigned long cMillis = pMillis;

  motor.write(motor_lock);
  while (b == LOW) {
    // periodicaly read unlock stop pin

    if (cMillis - pMillis >= motor_move_time) {
      // if 1 second has passed and motor isn't unlocked. Motor there is a
      // block in a way of pin
      // try to unlock
      unlock_flag = false; // we moved pin, somewere in the middle
      yield();

      if (unlock_sword() == true) {
        // stop motor, and raise critical error
        error_motor_cant_lock();
        return false;
      } else {
        return false;
      }
    }
    cMillis = millis();
    yield(); // wdt reset prevention
    b = digitalRead(PIN_IN);
  }

  motor.write(motor_stop);
  yield();

  unlock_flag = false;
  lock_flag = true;
  save_flags();
  Serial.println("Sword locked");

  return true;
}

bool unlock_sword() {
  bool b{0};
  b = digitalRead(PIN_OUT);

  if (unlock_flag == true) {
    Serial.println("Sword already unlocked");
    while (b == LOW) {
      motor.write(motor_stop - 5);
      yield();
      b = digitalRead(PIN_OUT);
      Serial.println("pin state OUT: " + (String)b);
    }
    motor.write(motor_stop);

    return true;
  }

  unsigned long pMillis{millis()};
  unsigned long cMillis{pMillis};

  Serial.println("Unlocking sword");
  motor.write(motor_unlock);

  while (b == LOW) {
    if (cMillis - pMillis >= motor_move_time) {
      // if second has passed and motor isnt unlocked, it is stuck raise
      motor.write(motor_stop + 2);
      yield();
      delay(50);

      error_motor_stuck();
      return false;
    }
    b = digitalRead(PIN_OUT);
    cMillis = millis();

    yield(); // prevent wdt reset
  }

  unlock_flag = true;
  lock_flag = false;
  motor.write(motor_stop);
  yield();
  save_flags();

  Serial.println("Sword unlocked");

  return true;
}