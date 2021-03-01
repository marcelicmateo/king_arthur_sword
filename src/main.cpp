#include <Arduino.h>
#include <stdlib.h>

// pin declarations
#define EM_TOGGLE 2 // D2
#define PIEZO_IN A0
#define LED_EM_RELEASE 4     // D4
#define LED_HUMAN_DETECTED 6 // D6
#define SWORD_DETECTED 8     // D8

void blink_led(int led_pin, int ms) {
  digitalWrite(led_pin, HIGH);
  delay(ms);
  digitalWrite(led_pin, LOW);
}

int check_if_sword_is_present(uint8_t sword_pin) {
  uint8_t is_detected = 0; // assume is present
  for (int i = 0; i < 3; i++) {
    is_detected |= digitalRead(sword_pin);
  }
  return ~is_detected;
}
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_EM_RELEASE, OUTPUT);
  pinMode(LED_HUMAN_DETECTED, OUTPUT);
  pinMode(EM_TOGGLE, OUTPUT);

  pinMode(PIEZO_IN, INPUT);
  pinMode(SWORD_DETECTED, INPUT_PULLUP); // default 1

  digitalWrite(LED_EM_RELEASE, 0);
  digitalWrite(LED_HUMAN_DETECTED, 0);
  digitalWrite(EM_TOGGLE, 0);

  while (!check_if_sword_is_present(
      SWORD_DETECTED)) { // sword not detected, wait forever
    digitalWrite(LED_EM_RELEASE, 1);
    delay(1000);
  };

  // when sword present, lock that MF
  digitalWrite(LED_EM_RELEASE, 0);
  digitalWrite(EM_TOGGLE, 1); // lock EM
}

#define MAX_PROBABILITY 100
#define PROBABILITY 19

// the loop function runs over and over again forever
void loop() {
  uint16_t piezo_value;
  uint8_t probability = PROBABILITY; // 100 max
  uint8_t random_number = 100;

  piezo_value = analogRead(PIEZO_IN); // check for little humans
  if (piezo_value > 1000) {           // human detected
    blink_led(LED_HUMAN_DETECTED, 500);
    random_number = rand() % MAX_PROBABILITY;
    if (random_number < probability) { // MY KING???
      digitalWrite(LED_EM_RELEASE, 1);
      digitalWrite(EM_TOGGLE, 0); // release EM
      delay(10 * 1000);           // 10 sec or nothing
    };
  }
  while (!check_if_sword_is_present( // now we play the waiting game
      SWORD_DETECTED)) {             // sword not detected, wait forever
    digitalWrite(LED_EM_RELEASE, 1);
    delay(1000);
  };
  // SUCKER, he gave sword back
  digitalWrite(LED_EM_RELEASE, 0);
  digitalWrite(EM_TOGGLE, 1); // lock that shit down
}
