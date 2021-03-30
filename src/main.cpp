#include "include.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG false
#define Serial \
  if (DEBUG)   \
  Serial

//unsigned long current_milis = millis();
//unsigned long previous_milis = 0;
//const long interval = 5000; // interval at which to blink (milliseconds)

void setup()
{
  Serial.begin(9600);
  //   defining pin modes
  pinMode(PIR_SENSOR, INPUT);
  pinMode(INFRARED_OBSTICLE_SENSOR, INPUT);
  pinMode(PIN_IN, INPUT);
  pinMode(PIN_OUT, INPUT);
  motor.attach(STEPER_MOTOR);

  wifi_setup();
  server_setup();

  if (!lock_flag && !unlock_flag)
  {
    unlock_sword();
  }
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
  static bool detection_flag{presistane_flag};
  detection_flag = is_human_detected();

  if (presistane_flag == false && detection_flag == true)
  {
    presistane_flag = true;
    if (is_sword_present())
    {
      lock_sword();

      if (random_chance_to_release())
      {
        is_king = true;
        unlock_sword();
      }
      else
      {
        is_king = false;
      }
    }
  }
  if (presistane_flag == true && detection_flag == false)
  {
    presistane_flag = false; // reset presistance flag
  }
  return;
}

unsigned long current_milis = millis();
unsigned long previous_milis = 0;
//const long interval = 5000; // interval at which to blink (milliseconds)
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
    current_milis = millis();
    if (current_milis - previous_milis <= 1000)
    {
      yield();
    }
    else
    {
      previous_milis = current_milis;
      normalan_nacin_rada();
    }
    break;
  default:
    break;
  }

  yield();
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
  //static unsigned long current_milis = millis();
  static bool b{false};
  b = (bool)digitalRead(PIR_SENSOR);

  return b;
}

bool random_chance_to_release()
{
  constexpr int MAX_RANDOM_NUMBER{99}; // constant
  static bool b{false};
  static unsigned int chance{0};
  chance = rand() % MAX_RANDOM_NUMBER;
  random_number_holder = chance;
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
  if (lock_flag == true)
  {
    Serial.println("Sword already locked");
    motor.writeMicroseconds(motor_stop);
  }
  else
  {
    Serial.println("Locking sword");
    motor.writeMicroseconds(motor_lock);
    b = digitalRead(PIN_IN);
    while (b == LOW)
    {
      b = digitalRead(PIN_IN);
      yield();
    }

    motor.writeMicroseconds(1800);
    yield();
    delay(100);
    motor.writeMicroseconds(motor_stop + 100);
    yield();
    delay(100);
    motor.writeMicroseconds(motor_stop);
    motor.writeMicroseconds(motor_stop);
  }
  unlock_flag = false;
  lock_flag = true;
  Serial.println("Sword locked");

  return;
}
void unlock_sword()
{
  bool b{0};
  b = digitalRead(PIN_OUT);
  if (unlock_flag == true)
  {
    Serial.println("Sword already unlocked");
    motor.writeMicroseconds(motor_stop);
  }
  else
  {
    Serial.println("Unlocking sword");
    motor.writeMicroseconds(motor_unlock);
    b = digitalRead(PIN_OUT);
    while (b == LOW)
    {
      b = digitalRead(PIN_OUT);
      yield();
    }

    motor.writeMicroseconds(1200);
    yield();
    delay(50);
    motor.writeMicroseconds(motor_stop - 100);
    yield();
    delay(100);
    motor.writeMicroseconds(motor_stop);
  }
  unlock_flag = true;
  lock_flag = false;
  Serial.println("Sword unlocked");

  return;
}