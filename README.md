# Parallel Computing - MidTerm Project

Morphological image processing with OpenMP parallelization.

## Description

Implements morphological opening (erosion + dilation) on grayscale images. Compares sequential vs parallel performance.

## Build

```bash
g++ -fopenmp -O2 -std=c++17 main.cpp sequential.cpp parallel.cpp -o main.exe
```

Or run `compile.bat`

## Usage

```bash
./main.exe [max_images] [num_threads]
```

## Files

- `main.cpp` - Main program
- `sequential.cpp/h` - Sequential operations
- `parallel.cpp/h` - OpenMP parallel operations
- `libs/` - STB image libraries
- `compile.bat` - Build script
