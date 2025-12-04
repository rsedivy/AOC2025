
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1000

const char DEFAULT_FILE[] = "puzzle/input.txt";

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

  char lineBuffer[BUF_SIZE] = {'\0'};
  unsigned int total = 0;

  while (!feof(input)) {
    int lineBufferLen = 0;

    fscanf(input, "%999s%n ", lineBuffer, &lineBufferLen);

    char digitString[3] = {'0', '0', '\0'};
    uint8_t firstNum = 0, secondNum = 0;

    for (int i = 0; i < lineBufferLen; ++i) {
      // Check for non-digit characters
      if (lineBuffer[i] < '0' || lineBuffer[i] > '9') {
        fprintf(stderr, "Unexpected character '%c' in input!", lineBuffer[i]);
        fclose(input);
        return EXIT_FAILURE;
      }

      uint8_t digit = (uint8_t)(lineBuffer[i] - 0x30); // Convert digit to int

      if (digit > firstNum) {
        if ((i + 1) < lineBufferLen) {
          firstNum = digit;
          // Since the numbers have to be sequential, we have to set a second
          // digit to the following one at first. This means the same number
          // will be evaluated next loop but it saves an extra assignment over
          // just skipping the next loop.
          uint8_t digit =
              (uint8_t)(lineBuffer[i + 1] - 0x30); // Convert next digit to int
          secondNum = digit;
        } else {
          secondNum = digit;
        }
      } else if (digit > secondNum) {
        secondNum = digit;
      }
    }

    digitString[0] = firstNum + 0x30;
    digitString[1] = secondNum + 0x30;

    total += atoi(digitString);
    // printf("%s\n%s\n", lineBuffer, digitString); // Debug message
  }

  printf("Total joltage: %u\n", total);

  return EXIT_SUCCESS;
}
