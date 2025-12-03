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
    if (lineBuffer[i] == '\n') {
      continue;
    }
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
      fprintf(stderr, "Unexpected character '%c' encountered!\n",
              lineBuffer[i]);
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

  // I'm kind of haphazardly switching between long types here. Too lazy to fix
  // it now, it works anyways.
  long long result = 0;
  long long resultPartTwo = 0;

  for (int i = 0; i < rangeAmount; ++i) {
    // printf("%ld - %ld\n", rangeArray[i][0], rangeArray[i][1]);

    for (uint64_t j = rangeArray[i][0]; j <= rangeArray[i][1]; ++j) {
      // printf("Evaluating %lu\n", j);
      //  get amount of digits
      int digits = (int)floorl(log10l((long double)j)) + 1;

      // PART 2
      // There's surely a faster way but this is the easiest naive algorithm
      // for me to implement.
      // I'm still avoiding string operations as numeric ones should run faster
      for (int k = 1; k <= (digits / 2); ++k) {
        // We evaulate for each possible pattern consisting of k digits that are
        // factors of the amount of digits in the evaluated number.
        if ((digits % k) != 0)
          continue;

        // This next part looks confusing but it's simple:
        // We are looping over the number always evaluating pairs of k digits in
        // a similar manner to how PART 1 is done.
        //
        // Example:
        // For evaluated number j=123123123 and k=3
        // We get the first segment of the pair (j % 100000000) / 100000 = 123
        // In this case 100000000 = 10^digits and 100000 = 10^digits-3
        //
        // Then, we compare it to the second segment: (j % 100000) / 100 = 123
        //
        // What we did here was compare the first three digits of j to the
        // second three digits. On the next iteration of the loop the same
        // happens, but we compare the second segment of three digits to the
        // third segment. If all comparisons are equal, the number is a
        // repeating pattern and is added to the result of the part two
        // calculation.
        int l = k;
        int equal = 1;
        do {
          uint64_t d1 = (long)pow(10, (double)digits - l + k);
          uint64_t d2 = (long)pow(10, (double)digits - l);
          // The modulo discards all digits before the segment, the divide
          // discards all digits after.
          uint64_t firstSegment = (j % d1) / d2;
          l += k;
          d1 = (long)pow(10, digits - l);
          d1 = (d1 == 0) ? 1
                         : d1; // To avoid division by zero on the last segment.
          // Note: d1 and d2 are flipped here to avoid another assignment. d2 is
          // upper bound, d1 is lower.
          uint64_t secondSegment = (j % d2) / d1;
          if (firstSegment != secondSegment) {
            equal = 0;
            break;
          }

        } while (l < digits);

        if (equal) {
          resultPartTwo += j;
          //  printf("Pattern 2 found: %lu\n", j);
          break;
        }
      }

      // PART 1
      // only numbers with even amounts of digits can repeat
      if ((digits % 2) != 0)
        continue;

      // We will divide and modulo by 10^(digits/2) to get the first and second
      // half. Example: 123123/1000 = 123; 123123%1000 == 123;
      uint64_t divisor = (long)pow(10, (double)digits / 2);
      if ((j / divisor) == (j % divisor)) {
        // printf("Pattern found: %lld\n", j);
        result += j;
      }
    }
  }

  printf("Result: %lld\n", result);
  printf("Result Part 2: %lld\n", resultPartTwo);

  return EXIT_SUCCESS;
}
