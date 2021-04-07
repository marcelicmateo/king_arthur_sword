#include "error.h"
#include "include.h"
#include "pin.h"
#include "server_setup.h"
#include "state.h"
#include "sword.h"
#include "wifi_setup.h"
#include <cstdlib>

#define DEBUG true
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial

constexpr char BINGO_DATA[]{"Bingo.txt"};
uint16 numberOfContestants{0};
uint16 numberOfWinnings{0};
constexpr uint16 NUMBER_OF_WINNERS{50};
int winningNumbers[NUMBER_OF_WINNERS] = {0};

void write_to_file(String s, String file_name) {
  Serial.println("Writing to file.\n" + s);

  File f = LittleFS.open(file_name, "w");
  for (uint i = 0; i < s.length(); i++) {
    f.write(s[i]);
  }
  f.close();
}

String read_from_file(String file_name) {
  String buff{""};
  if (LittleFS.exists(file_name)) {
    File f = LittleFS.open(file_name, "r");
    buff = f.readString();
    Serial.println("Read: " + buff);
    f.close();
  } else {
    Serial.println("Error reading.\nFile >" + file_name + "< does not exist");
  }
  return buff;
}

void generate_bingo_winners() {
  pinMode(A0, INPUT);
  randomSeed(analogRead(A0)); // random noise on adc for random seed
  Serial.println("Generated random numbers");
  for (int i = 0; i < numberOfWinnings; i++) {
    winningNumbers[i] = (int)random(1, numberOfContestants + 1);
    Serial.println((String)i + ": " + (String)winningNumbers[i]);
  }
  return;
}

void get_bingo_data() {
  String data = read_from_file(BINGO_DATA);
  if (data == "") {
    Serial.println("Generateing new bingo data manualy.");
    return;
  }
  String buff{""};
  int j{0};
  for (uint i = 0; i < data.length(); i++) {
    if (data[i] != ';') {
      buff += data[i];
    } else {
      if (j == 0) {
        numberOfContestants = buff.toInt();
      } else {
        winningNumbers[j - 1] = buff.toInt();
      }
      j++;
      buff = "";
    }
    Serial.println("buff: " + buff);
    Serial.println("j :  " + (String)j);
    Serial.println(
        "w Num: " +
        (String)winningNumbers[j - 1]); // garbage on j == 0, take care
  }
  numberOfWinnings = j - 1;
  return;
}

void setup() {
  Serial.begin(115200);

#ifdef DEBUG
  yield();
  delay(500); // get serial up
#endif
  Serial.println("SETUP BEGIN");
  init_gpio();
  motor.attach(STEPER_MOTOR);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  get_bingo_data();
  Serial.println("Contestans " + (String)numberOfContestants);
  Serial.println("N of Winenrs: " + (String)numberOfWinnings);
  for (int i = 0; i < numberOfWinnings; i++) {
    Serial.println(winningNumbers[i]);
  }

  wifi_setup();
  server_setup();

  if (!lock_flag && !unlock_flag) {
    unlock_sword();
  }
  Serial.println("Begin loop");
}

unsigned long current_milis = millis();
unsigned long previous_milis = 0;

void loop() {

  AsyncElegantOTA.loop(); // service OTA updates

  switch (state) { // state machine
  case 127:
    // critical error, system halt
    Serial.println("Critical error, system halt");
    break;
  case 126:
    // lock error
    Serial.println("Sword cant be locked");
    break;
  case 1:
    sword_prema_unlocked();
    break;
  case 2:
    sword_prema_locked();
    break;
  case 3:
    bingo();
    break;
  default:
    // 0
    // should never come to this value
    // wdt reset hapend
    Serial.println("Error, reseting system");
    yield();
    ESP.restart();
  }

  yield(); // let ESP do stuff
  delay(200);
}
