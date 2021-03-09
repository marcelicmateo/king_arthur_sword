#include <Arduino.h>
#include <stdlib.h>

#define DEBUG false
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial
// pin declaration
// analog pins
#define POT_IN A2
#define PIEZO_IN A0

// digital
#define EM_TOGGLE 2          // D2, toggle EM power
#define LED_EM_POWER 4       // D4
#define LED_HUMAN_DETECTED 6 // D6
#define IS_SWORD_DETECTED 8  // D8, NEGATIVE logic

void blink_led(int led_pin, int ms) {
  digitalWrite(led_pin, HIGH);
  delay(ms);
  digitalWrite(led_pin, LOW);
}

uint8_t check_if_sword_is_present(uint8_t sword_pin) {
  // neds to sit 1.5 sec in place
  uint8_t is_detected = 0; // assume is present
  for (int i = 0; i < 3; i++) {
    is_detected |= digitalRead(sword_pin);
    delay(.5 * 1000); // removes debouncing pozitives
  }
  Serial.println("Sword detected? - " + (String)(is_detected ? "NO" : "YES"));
  return is_detected;
}

void wait_for_sword() {
  while (check_if_sword_is_present(IS_SWORD_DETECTED) ==
         1) { // sword not detected, wait forever
    Serial.println("No sword");
  };
}

void setup() {
  Serial.begin(9600);
  // output pins
  pinMode(LED_EM_POWER, OUTPUT);
  pinMode(LED_HUMAN_DETECTED, OUTPUT);
  pinMode(EM_TOGGLE, OUTPUT);
  // input pins
  pinMode(PIEZO_IN, INPUT);
  pinMode(PIEZO_IN, INPUT);
  pinMode(IS_SWORD_DETECTED, INPUT_PULLUP); // default HIGH

  // seting default pin values
  digitalWrite(EM_TOGGLE, LOW);          // EM power off
  digitalWrite(LED_EM_POWER, LOW);       // EM power off
  digitalWrite(LED_HUMAN_DETECTED, LOW); // no human detected

  wait_for_sword();

  // when sword present, lock that MF
  digitalWrite(LED_EM_POWER, LOW);
  digitalWrite(EM_TOGGLE, HIGH); // lock EM, power ON
}

// since probability is dependent on linear resistant potentiometer
// sampled from ADC. Max value from adc coresponds with max value of random
// number generator

#define RAND_DIV 1020 // 100% in probability
#define MIN_PROBABILITY 34 // every 5, 20%, min pot value
#define MAX_PROBABILITY 204 // every 30, 3%, max pot value

uint16_t pot_to_pobability_border(uint16_t pot_value) {
  static float scale{(MAX_PROBABILITY - MIN_PROBABILITY) /
                     1023.0f}; // calculate once
  uint16_t prob_scaled{1024};
  prob_scaled =
      static_cast<uint16_t>(round(scale * pot_value + MIN_PROBABILITY));
  return prob_scaled;
}

void loop() {
  // always have def values
  uint16_t piezo_value{0};
  uint16_t probability{MIN_PROBABILITY};
  uint16_t random_number{100}; // random from my head

  // sefety check
  // cover all unvanted states

  // is sword present?
  if (check_if_sword_is_present(IS_SWORD_DETECTED) == 0 &&
      digitalRead(EM_TOGGLE) == 1) { // no sword but EM is ON, not good
    digitalWrite(EM_TOGGLE, LOW);
    digitalWrite(LED_EM_POWER, LOW);
    wait_for_sword();
  }

  piezo_value += analogRead(PIEZO_IN); // check for little humans
  piezo_value += analogRead(PIEZO_IN); // check for little humans
  piezo_value += analogRead(PIEZO_IN); // check for little humans
  piezo_value = piezo_value / 3;

  Serial.println("Piezo, read: " + (String)piezo_value);

  if (piezo_value > 1000) { // human detected
    blink_led(LED_HUMAN_DETECTED, 500);

    random_number = rand() % RAND_DIV; // do we let him be the King?
    probability = pot_to_pobability_border(
        analogRead(POT_IN)); // linear dependency of pot value to probabilty

    Serial.println("Random number: " + (String)random_number);
    Serial.println("Probabity: " + (String)probability);

    if (random_number < probability) { // MY KING???
      digitalWrite(LED_EM_POWER, 1);
      digitalWrite(EM_TOGGLE, 0); // release EM

      delay(10 * 1000); // 10 sec or nothing
      wait_for_sword();

      // SUCKER, he gave sword back
      digitalWrite(LED_EM_POWER, LOW);
      digitalWrite(EM_TOGGLE, HIGH); // lock that s*** down
    }
  }
}
