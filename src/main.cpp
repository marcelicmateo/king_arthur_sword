#include <Arduino.h>
#include <stdlib.h>

// pin declarations
#define EM_TOGGLE 2 // D2
#define POT_IN A2
#define PIEZO_IN A0
#define LED_EM_RELEASE 4     // D4
#define LED_HUMAN_DETECTED 6 // D6
#define SWORD_DETECTED 8
// D8

void blink_led(int led_pin, int ms) {
  digitalWrite(led_pin, HIGH);
  delay(ms);
  digitalWrite(led_pin, LOW);
}

uint8_t check_if_sword_is_present(uint8_t sword_pin) {
  uint8_t is_detected = 0; // assume is present
  for (int i = 0; i < 3; i++) {
    is_detected |= digitalRead(sword_pin);
  }
  Serial.println("Is detected?");
  Serial.println(is_detected);
  return is_detected;
}
void setup() {
  Serial.begin(9600);
  pinMode(LED_EM_RELEASE, OUTPUT);
  pinMode(LED_HUMAN_DETECTED, OUTPUT);
  pinMode(EM_TOGGLE, OUTPUT);
  pinMode(PIEZO_IN, INPUT);

  pinMode(PIEZO_IN, INPUT);
  pinMode(SWORD_DETECTED, INPUT_PULLUP); // default 1

  digitalWrite(LED_EM_RELEASE, LOW);
  digitalWrite(LED_HUMAN_DETECTED, LOW);
  digitalWrite(EM_TOGGLE, LOW);

  while (check_if_sword_is_present(SWORD_DETECTED) ==
         1) { // sword not detected, wait forever
    digitalWrite(LED_EM_RELEASE, HIGH);
    Serial.println("No sword");
    delay(1000);
  };

  // when sword present, lock that MF
  digitalWrite(LED_EM_RELEASE, LOW);
  digitalWrite(EM_TOGGLE, HIGH); // lock EM
}

#define RAND_DIV 1020
#define MIN_PROBABILITY 34
#define MAX_PROBABILITY 204
// the loop function runs over and over again forever
void loop() {
  uint16_t piezo_value;
  uint8_t probability = MIN_PROBABILITY; // 100 max
  uint8_t random_number = 100;

  piezo_value = analogRead(PIEZO_IN); // check for little humans
  Serial.print("Piezo, read: ");
  Serial.println(piezo_value);
  if (piezo_value > 1000) { // human detected
    blink_led(LED_HUMAN_DETECTED, 500);
    random_number = rand() % RAND_DIV;
    Serial.print("Random number");
    Serial.println(random_number);
    probability = analogRead(POT_IN);
    Serial.println("Probabity: ");
    Serial.println(probability);
    if (random_number < probability) { // MY KING???
      digitalWrite(LED_EM_RELEASE, 1);
      digitalWrite(EM_TOGGLE, 0); // release EM

      delay(10 * 1000); // 10 sec or nothing
      while (check_if_sword_is_present(SWORD_DETECTED) == 1)
        ;
    }
  }
  // SUCKER, he gave sword back
  digitalWrite(LED_EM_RELEASE, LOW);
  digitalWrite(EM_TOGGLE, HIGH); // lock that shit down
}
