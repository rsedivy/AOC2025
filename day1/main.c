#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Puzzle input is ~4100, I'm not going to bother with a dynamic array
#define INPUT_SIZE 6000

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
      printf(stderr, "Puzzle input not found\n");
    } else {
      printf(stderr, "Error opening file\n");
    }

    return EXIT_FAILURE;
  }

  int state = 50; // Initial dial state
  int zeroCount = 0;

  for (int i = 0; i < INPUT_SIZE; ++i) {
    int mul = 1;
    int value = 0;
    char sign[2];

    if (fscanf(input, "%1[RL]%d ", sign, &value) != 2) {
      if (feof(input)) {
        break;
      } else {
        if (ferror(input)) {
          printf(stderr, "Input error occured\n");
        }
        fclose(input);
        return EXIT_FAILURE;
      }
    }

    // printf("%c%d\n", sign[0], value);

    mul = (sign[0] == 'L') ? -1 : 1;

    value *= mul;

    // printf("Value: %d, Current state: %d, ", value, state);

    state = (state + value + 100) % 100;

    // printf("New state: %d\n", state);

    if (state == 0)
      zeroCount++;
  }

  printf("Password: %d\n", zeroCount);
  if (fclose(input) != 0) {
    printf(stderr,
           "Warning: unable to close file. Something went very wrong. \n");
  }

  return EXIT_SUCCESS;
}
