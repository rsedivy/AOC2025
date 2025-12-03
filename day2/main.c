/**
 * Day 2 Part 1
 */
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Challenge input is 500 characters, this is for testing.
#define BUF_SIZE 1000

const char DEFAULT_FILE[] = "puzzle/input.txt";

enum STATE { FIRST_NUM, SECOND_NUM };

void storeNumber(enum STATE state, uint64_t rangeArray[250][2], int index,
                 uint64_t n) {
  int j = (state == FIRST_NUM) ? 0 : 1;
  rangeArray[index][j] = n;
  return;
}

// Read input in single loop for speed.
// Reading is done as a state machine.
// When in state FIRST_NUM, it will read until the first dash
// When in state SECOND_NUM, it will read until the next comma
// Upon reaching a state transition it will write the number to the range
// array String terminator is only expected in state SECOND_NUM, error
// otherwise.
// UPDATE: I kind of one-shotted myself here. The time spent on this
// overcomplicated parsing function was entirely wasted.
// Execution time savings are minimal compared to the actual computation.
// I had so many off-by-one and overflow bugs that I didn't even notice
// that some of the input numbers didn't fit into int32 which wasted like two
// hours (Incidentally that's why I've scattered so many different long types
// across the code and I'm too lazy to fix it now)
int parseInput(uint64_t rangeArray[250][2], int *rangeAmount,
               char lineBuffer[BUF_SIZE]) {

  char numberBuffer[100];
  int numBufIndex = 0;
  enum STATE currentState = FIRST_NUM;
  int rangeIndex = 0; // Current range index.

  for (int i = 0; i < BUF_SIZE; ++i) {

    if (lineBuffer[i] == '\0') {
      if (currentState != SECOND_NUM) {
        // error condition
        fprintf(stderr, "Input line ended too early!\n");
        return 1;
      }
      if (numBufIndex == 0) {
        fprintf(stderr, "Last number is empty!");
        return 1;
      }
      numberBuffer[numBufIndex] = '\0';
      storeNumber(currentState, rangeArray, rangeIndex, atoll(numberBuffer));
      numBufIndex = 0;
      *rangeAmount += 1;
      // write number to range array and exit loop
      break;
    }

    if (lineBuffer[i] == '-') {
      if (currentState != FIRST_NUM) {
        fprintf(stderr, "'-' character encountered too early!\n");
      }
      // Write number to range array and continue.
      numberBuffer[numBufIndex] = '\0';
      storeNumber(currentState, rangeArray, rangeIndex, atoll(numberBuffer));
      currentState = SECOND_NUM;
      numBufIndex = 0;
      continue;
    }

    if (lineBuffer[i] == ',') {
      if (currentState != SECOND_NUM) {
        fprintf(stderr, "',' character encountered too early!\n");
      }
      // Write number to range array and continue to next one.

      numberBuffer[numBufIndex] = '\0';
      storeNumber(currentState, rangeArray, rangeIndex, atoll(numberBuffer));
      currentState = FIRST_NUM;
      numBufIndex = 0;
      *rangeAmount += 1;
      rangeIndex++;
      continue;
    }

    if (lineBuffer[i] < '0' || lineBuffer[i] > '9') {
      fprintf(stderr, "Unexpected character '%c' encountered!", lineBuffer[i]);
      return 1;
    }

    numberBuffer[numBufIndex] = lineBuffer[i];
    numBufIndex++;
  }
  return 0;
}

int main(int argc, char *argv[]) {

  FILE *input;
  // Open file based on command line input, or default file if not supplied.
  if (argc > 1) {
    input = fopen(argv[1], "r");
  } else {
    input = fopen(DEFAULT_FILE, "r");
  }

  // Check if file opened correctly. Check for file not found error.
  if (!input) {
    if (errno == ENOENT || errno == EACCES) {
      fprintf(stderr, "Puzzle input not found\n");
    } else {
      fprintf(stderr, "Error opening file\n");
    }

    return EXIT_FAILURE;
  }

  // Input is only single line, read the line.
  char lineBuffer[BUF_SIZE] = {'\0'};

  // range array
  uint64_t rangeArray[250][2]; // Array to store each range.
  int rangeAmount = 0;         // Amount of number ranges read from input

  // Expecting input to be single line. Newlines and terminating newlines will
  // break it.
  if (fgets(lineBuffer, BUF_SIZE, input) == NULL) {
    fclose(input);
    fprintf(stderr, "Error reading input line.\n");
    return EXIT_FAILURE;
  }

  // Parsed input gets passed into range array
  if (parseInput(rangeArray, &rangeAmount, lineBuffer) == 1) {
    fclose(input);
    return EXIT_FAILURE;
  }

  long long result =
      0; // I'm kind of haphazardly switching between long types here.
  // Too lazy to fix it now, it works anyways.

  for (int i = 0; i < rangeAmount; ++i) {
    // printf("%ld - %ld\n", rangeArray[i][0], rangeArray[i][1]);

    for (uint64_t j = rangeArray[i][0]; j <= rangeArray[i][1]; ++j) {
      // get amount of digits
      int digits = (int)floorl(log10l((long double)j)) + 1;
      // only numbers with even amounts of digits can repeat
      if ((digits % 2) != 0)
        continue;

      // We will divide and modulo by 10^(digits/2) to get the first and second
      // half. Example: 1010/100 = 10; 1010%100 == 10;
      uint64_t divisor = (long)pow(10, (double)digits / 2);
      if ((j / divisor) == (j % divisor)) {
        // printf("Pattern found: %lld\n", j);
        result += j;
      }
    }
  }

  printf("Result: %lld\n", result);

  return EXIT_SUCCESS;
}
