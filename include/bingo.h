#pragma once
#include "include.h"

#include <ESP8266TrueRandom.h>

constexpr uint16 NUMBER_OF_WINNERS{100};

constexpr char BINGO_DATA[]{"Bingo.txt"};
constexpr char CURRENT_PLAYER_txt[]{"CurrentPlayer.txt"};

uint16 numberOfContestants{0};
uint16 numberOfWinnings{0};
unsigned int winningNumbers[NUMBER_OF_WINNERS] = {0};
unsigned int currentPlayer{0};

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

void sort(unsigned int *a, int n) {
  unsigned int holder;
  for (int x = 0; x < numberOfWinnings; x++)
    for (int y = 0; y < numberOfWinnings - 1; y++)
      if (a[y] > a[y + 1]) {
        holder = a[y + 1];
        a[y + 1] = a[y];
        a[y] = holder;
      }
}

void change_current_player(int c) {
  currentPlayer = c;
  write_to_file((String)currentPlayer, CURRENT_PLAYER_txt);
  return;
}

void generate_bingo_winners() {
  // pinMode(A0, INPUT);
  // random noise on adc for random seed
  // randomSeed(analogRead(A0));

  change_current_player(0);
  // String buf{(String)numberOfContestants + ";" + (String)currentPlayer +
  // ";"};
  String save{(String)numberOfContestants + ";"};
  Serial.println("Generated random numbers");
  unsigned int tmp{0};
  bool done{true};
  if (numberOfWinnings > numberOfContestants) {
    numberOfWinnings = numberOfContestants;
  }
  Serial.println("N WINERS: " + (String)numberOfWinnings +
                 "\nN of People: " + (String)numberOfContestants);
  Serial.println("Generated number: ");
  for (int i = 0; i < numberOfWinnings; i++) {

    do { // generate unique numbers
      tmp = (unsigned int)ESP8266TrueRandom.random(1, numberOfContestants + 1);
      done = false;
      Serial.print((String)tmp + " ");
      for (int j = 0; j < i; j++) {
        if (winningNumbers[j] == tmp) {
          done = true;
        }
      }
    } while (done);
    Serial.println();
    winningNumbers[i] = tmp;
    Serial.println((String)i + ": " + (String)winningNumbers[i]);
  }

  sort(winningNumbers, numberOfWinnings);
  for (int i = 0; i < numberOfWinnings; i++) {
    // buf += (String)winningNumbers[i] + ";";
    save += (String)winningNumbers[i] + ";";
  }
  write_to_file(save, BINGO_DATA);

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

  data = read_from_file(CURRENT_PLAYER_txt);
  currentPlayer = data.toInt();
  Serial.println("Current player: " + (data));
  return;
}