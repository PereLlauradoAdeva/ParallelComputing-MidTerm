# Parallel Image Processing with OpenMP

Mid-term project for Parallel Computing course.

## Description

Implements **morphological opening** (erosion + dilation) on grayscale images using OpenMP parallelization. Compares sequential vs parallel performance and calculates speedup.

## Build

```bash
g++ -fopenmp -O2 -o morphological main.cpp
```

## Run

```bash
./morphological [max_images] [num_threads]
```

## Files

- `main.cpp` - Source code with sequential and parallel implementations
- `libs/` - STB image libraries for image I/O
