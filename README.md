# Parallel Computing - MidTerm Project

Morphological image processing with OpenMP parallelization.

## Description

Implements morphological opening (erosion + dilation) on grayscale images. Compares sequential vs parallel performance across different thread counts.

## Build

```bash
g++ -fopenmp -O2 -std=c++17 main.cpp sequential.cpp parallel.cpp -o main.exe
```

Or run `compile.bat`

## Usage

The program tests performance with different thread counts (1, 2, 4, 8, 12, 16) on two image sets:

```bash
./main.exe [max_images1] [max_images2]
```

- `max_images1`: Number of images from `input_images` (-1 for all)
- `max_images2`: Number of images from `input_images2` (-1 for all)

Example:
```bash
./main.exe -1 -1    # Process all images from both folders
./main.exe 10 5     # Process 10 from input_images, 5 from input_images2
```

## Output

- `performance_results_1.csv` - Results for input_images
- `performance_results_2.csv` - Results for input_images2
- `output_images/` - Processed images from input_images
- `output_images2/` - Processed images from input_images2

CSV files contain: Threads, Sequential Time, Parallel Time, Speedup, and Efficiency data.

## Files

- `main.cpp` - Performance testing program
- `main_original.cpp` - Original demo version
- `sequential.cpp/h` - Sequential operations
- `parallel.cpp/h` - OpenMP parallel operations
- `libs/` - STB image libraries
- `compile.bat` - Build script
