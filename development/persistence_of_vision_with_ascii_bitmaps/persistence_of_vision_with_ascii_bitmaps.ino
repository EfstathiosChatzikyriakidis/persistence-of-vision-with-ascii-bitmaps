/*
 *  Persistence Of Vision With ASCII Bitmaps.
 *
 *  Copyright (C) 2010 Efstathios Chatzikyriakidis (stathis.chatzikyriakidis@gmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// include ASCII character set (only printable characters).
#include "ascii_bitmap_7x5.h"

const long debounceTime = 120; // delay time before writing the next character.
const long colDelayTime = 400; // delay time before writing next char's column.

long lastSavePointTime = 0; // used for storing a savepoint time (achieve time measurement).

// LEDS' PINS numbers.
const int povPins[charHeight] = {
  9, 8, 7, 6, 5, 4, 3
};

// the message we want to print.
const char msgBuffer[] = "Happy Hacking !";

// the length of the message (without the null-terminated character).
const int msgSize =
  (int) (sizeof (msgBuffer) / sizeof (msgBuffer[0]) - 1);

// startup point entry (runs once).
void setup() {
  // set each LED PIN as an output.
  for (int n = 0; n < charHeight; n++) {
    pinMode(povPins[n], OUTPUT);
  }
}

// print the column of a character.
void printCharColumn(byte data) {
  // in order to print the column of a character we set each LED individually,
  // the algorithm used to get each LED's value from the character's column is
  // known as "Walking 1's" algorithm which is used also for memory checking.
  
  int pinIndex = 0; // each column's bit value will go to a different LED.

  // start "Walking 1's" algorithm to fetch and write each LED's value.
  for (byte mask = B00000001; mask < 128; mask <<= 1) { // ignore MSB: eighth bit.
    // write LED's value.
    digitalWrite(povPins[pinIndex], mask & data);
    
    // next time assume the next LED PIN.
    pinIndex++;
  }
}

// return the index of the character from the ASCII character set.
int getCharIndex(char ch) {
  // security issue: check bounds for unknown characters.
  if (ch < 32 || ch > 126) {
    ch = 32; // if the character is unknown assume it as a blank character.
  }

  // return the index (calculate by offset) of the character.
  return (ch - 32);
}

// print a character.
void printChar(char aChar) {
  // get the index of the character from the ASCII character set.
  int charSetIndex = getCharIndex(aChar);

  // print each column of the character.
  for (int n = 0; n < charWidth; ++n) {
    // print the current column of the character.
    printCharColumn(ASCIIPrintable[charSetIndex][n]);

    // wait some time between columns.
    delayMicroseconds(colDelayTime);
  }

  // print a blank column.
  printCharColumn(ASCIIPrintable[0][0]);

  // wait some time before next character.
  delayMicroseconds(colDelayTime);
}

// print each character of the message.
void printMsg() {
  // loop through the characters of the message.
  for (int n = 0; n < msgSize; ++n) {
    // print the current character.
    printChar(msgBuffer[n]);
  }
}

// loop the main sketch.
void loop() {
  // if it is time to print the message again do it.
  if (abs(millis() - lastSavePointTime) > debounceTime) {
    // store a new savepoint time for future reference.
    lastSavePointTime = millis();

    // print the message.
    printMsg();
  }
}
