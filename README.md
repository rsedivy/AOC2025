# AOC2025 - rsedivy

My Advent of Code 2025 solutions.

I'm not code golfing or focusing on optimization this year, instead I'm trying to make robust and readable code.

## Build

### Building all

```zsh
cmake -S . -B build
cmake --build build
```

### Building only a specific day's challenge

```zsh
cmake -S . -B build -DDAY=day[n]
cmake --build build
```

## Usage

Challenge executables will be built in `./day[n]/bin/day[n]`.

When run without options, most of them will expect to find the challenge input in `./puzzle/input.txt` (From the directory where it is being run.).

For most of them, you can also specify the input by passing it as a command line argument:  
`./day1/bin/day1 input1.txt # Example`
