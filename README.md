# Parallel Image Processing with OpenMP

## Overview

This project implements **morphological image processing operations** using **OpenMP** for parallel computing. It demonstrates the performance benefits of parallelizing image processing algorithms by comparing sequential and parallel implementations.

## What it Does

The program applies **morphological opening** (erosion followed by dilation) to grayscale images. Morphological opening is used to:
- Remove small bright spots (noise)
- Smooth object contours
- Separate loosely connected objects

### Key Features

- **Parallel vs Sequential comparison**: Both implementations are timed to calculate speedup
- **OpenMP parallelization**: Uses `#pragma omp parallel for collapse(2)` for data parallelism
- **Batch processing**: Processes multiple images from an input directory
- **Performance metrics**: Reports per-image and overall speedup ratios

## Operations Implemented

1. **Dilation**: Expands bright regions (finds maximum in kernel neighborhood)
2. **Erosion**: Shrinks bright regions (finds minimum in kernel neighborhood)
3. **Opening**: Erosion followed by dilation (removes small bright spots)

## Building

Requires a C++ compiler with OpenMP support.

```bash
# Linux/macOS with g++
g++ -fopenmp -O2 -o morphological main.cpp

# Windows with MSVC
cl /openmp /O2 main.cpp
```

## Usage

```bash
./morphological [max_images] [num_threads]
```

- `max_images`: Number of images to process (-1 for all)
- `num_threads`: Number of OpenMP threads to use

**Example:**
```bash
./morphological 10 4  # Process 10 images using 4 threads
```

## Project Structure

```
.
├── main.cpp              # Main source code with all implementations
├── libs/
│   ├── stb_image.h       # Image loading library
│   └── stb_image_write.h # Image writing library
├── input_images/         # Place input images here (jpg/png)
└── output_images/        # Processed images are saved here
```

## Dependencies

- **STB Image libraries** (included in `libs/`): Single-header libraries for image I/O
- **OpenMP**: For parallel processing

## Output

The program outputs:
- Processing time for each image (sequential and parallel)
- Per-image speedup ratio
- Total processing time comparison
- **Global speedup** across all processed images

## Academic Context

This is a mid-term project for a **Parallel Computing** course, demonstrating:
- Data parallelism concepts
- OpenMP directives and scheduling
- Performance analysis and speedup calculation
- Practical application of parallel algorithms to image processing
