#pragma once
#include "include.h"

constexpr uint16 NUMBER_OF_WINNERS{50};

constexpr char BINGO_DATA[]{"Bingo.txt"};

uint16 numberOfContestants{0};
uint16 numberOfWinnings{0};
int winningNumbers[NUMBER_OF_WINNERS] = {0};
int currentPlayer{0};

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

void sort(int *a, int n) {
  int holder;
  for (int x = 0; x < numberOfWinnings; x++)
    for (int y = 0; y < numberOfWinnings - 1; y++)
      if (a[y] > a[y + 1]) {
        holder = a[y + 1];
        a[y + 1] = a[y];
        a[y] = holder;
      }
}

String generate_bingo_winners() {
  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));
  currentPlayer = 0; // random noise on adc for random seed
  String buf{(String)numberOfContestants + ";" + (String)currentPlayer + ";"};
  Serial.println("Generated random numbers");
  int tmp{0};
  bool done{true};
  if (numberOfWinnings > numberOfWinnings) {
    numberOfWinnings = numberOfContestants;
  }
  Serial.println("Generated number: ");
  for (int i = 0; i < numberOfWinnings; i++) {
    do { // generate unique numbers
      tmp = (int)random(1, numberOfContestants + 1);
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
    buf += (String)winningNumbers[i] + ";";
  }
  write_to_file(buf, BINGO_DATA);

  return buf;
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
      } else if (j == 1) {
        currentPlayer = buff.toInt();
      } else {
        winningNumbers[j - 2] = buff.toInt();
      }
      j++;
      buff = "";
    }
    Serial.println("buff: " + buff);
    Serial.println("j :  " + (String)j);
    Serial.println(
        "w Num: " +
        (String)winningNumbers[j - 2]); // garbage on j == 0, take care
  }
  numberOfWinnings = j - 2;
  return;
}