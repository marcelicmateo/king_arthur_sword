#include "include.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG false
#define Serial \
  if (DEBUG)   \
  Serial

unsigned long current_milis = millis();
unsigned long previous_milis = 0;
const long interval = 5000; // interval at which to blink (milliseconds)

void setup()
{
  Serial.begin(9600);
  //   defining pin modes
  pinMode(PIR_SENSOR, INPUT);
  pinMode(INFRARED_OBSTICLE_SENSOR, INPUT);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, INPUT);
  motor.attach(STEPER_MOTOR);
  unlock_sword();

  wifi_setup();
  server_setup();
  Serial.println("Begin loop");
}
void motor_upotpunosti_otkljucan()
{
  Serial.println("Nacin rada 1");
  unlock_sword();
  return;
}

void motor_upotpunosti_zakljucan()
{
  Serial.println("Nacin rada 2");
  lock_sword();
  return;
}

void normalan_nacin_rada()
{
  Serial.println("Normalan nacin rada");
  if (is_sword_present())
  {
    lock_sword();
    if (is_human_detected())
    {
      if (random_chance_to_release())
      {
        unlock_sword();
        yield();
        delay(5000);
      }
    }
  }
  return;
}

void loop()
{
  AsyncElegantOTA.loop();

  switch (nacin_rada)
  {
  case 1:
    motor_upotpunosti_otkljucan();
    break;
  case 2:
    motor_upotpunosti_zakljucan();
    break;
  case 3:
    normalan_nacin_rada();
    break;
  default:
    break;
  }

  yield();
  delay(20);
}

bool is_sword_present()
{
  static bool b{true};
  b = (bool)digitalRead(INFRARED_OBSTICLE_SENSOR) ^
      1; // negative logic senor so XOR for positive logic
  return b;
}
bool is_human_detected()
{
  static bool b{false};
  b = (bool)digitalRead(PIR_SENSOR);
  if (b == true)
  {
    current_milis = millis();
  }
  if (current_milis - previous_milis >= interval)
  {
    // save the last time you blinked the LED
    previous_milis = current_milis;
    return false;
  }
  return b;
}

bool random_chance_to_release()
{
  constexpr int MAX_RANDOM_NUMBER{99}; // constant
  static bool b{false};
  static unsigned int chance{0};
  chance = rand() % MAX_RANDOM_NUMBER;
  Serial.println("Chance to be a king: " + (String)chance);
  if (chance < probability_to_pass)
    b = true;
  else
    b = false;
  return b;
}

void lock_sword()
{
  bool b{0};
  b = digitalRead(PIN_IN);
  if (b == HIGH)
  {
    Serial.println("Sword already locked");
    motor.write(motor_stop);
  }
  else
  {
    Serial.println("Locking sword");
    motor.write(motor_lock);
    b = digitalRead(PIN_IN);
    while (b == LOW)
    {
      b = digitalRead(PIN_IN);
      yield();
    }
    motor.write(motor_stop);
  }
  Serial.println("Sword locked");

  return;
}
void unlock_sword()
{
  bool b{0};
  b = digitalRead(PIN_OUT);
  if (b == HIGH)
  {
    Serial.println("Sword already unlocked");
    motor.write(motor_stop);
  }
  else
  {
    Serial.println("Unlocking sword");
    motor.write(motor_unlock);
    b = digitalRead(PIN_OUT);
    while (b == LOW)
    {
      b = digitalRead(PIN_OUT);
      yield();
    }
    motor.write(motor_stop);
  }
  Serial.println("Sword unlocked");

  return;
}