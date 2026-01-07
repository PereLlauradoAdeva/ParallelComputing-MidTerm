# Parallel Image Processing Project

This project implements a parallelized version of Morphological Opening (Erosion followed by Dilation) using OpenMP in C++. It compares the performance of **Static** vs **Dynamic** scheduling across different thread counts and datasets.

## 📌 Overview
The goal is to demonstrate the speedup and efficiency gained by parallelizing image processing tasks on multi-core processors. The project processes batches of images, applying a 9x9 kernel for morphological operations, and verifies the results against a sequential baseline.

## 🚀 Build & Run

### Prerequisites
*   **C++ Compiler**: `g++` (MinGW) with OpenMP support (`-fopenmp`).
*   **Python 3**: For generating performance plots (requires `pandas`, `matplotlib`).

### Compilation
You can compile the project using the provided batch script or manually via the terminal:

**Using Script:**
```cmd
compile.bat
```

**Manual Compilation:**
```cmd
g++ -fopenmp -O2 -std=c++17 main.cpp sequential.cpp parallel.cpp -o main.exe
```

### Execution
Run the compiled executable. You can optionally limit the number of images processed for each dataset (pass `-1` to process all).

```cmd
main.exe [max_images_dataset1] [max_images_dataset2]
```

**Example:**
```cmd
main.exe 10 10  # Process only 10 images from each dataset
main.exe -1 -1  # Process ALL images (default)
```

### Visualization
After running `main.exe`, result CSV files are generated. Run the Python script to create performance graphs:
```cmd
python plot_results.py
```

## ⚙️ Parameters & Configuration

| Parameter | Value | Description |
| :--- | :--- | :--- |
| **Kernel Size** | 9x9 | Size of the structuring element for Erosion/Dilation. |
| **Threads** | 1, 2, 4, 8, 16, 32 | Number of OpenMP threads tested. |
| **Scheduling** | Static, Dynamic | OpenMP scheduling policies compared. |
| **Operations** | Opening | Erosion followed by Dilation. |

## 📂 Datasets used
The project uses two distinct datasets to evaluate performance under different loads:

1.  **Dataset 1 (Large Images)**: `input_images`
    *   **Source**: **IDRiD** (Indian Diabetic Retinopathy Image Dataset).
    *   **Content**: High-resolution retinal images.
    
2.  **Dataset 2 (Small Images)**: `input_images2`
    *   **Source**: **Satellite Images of Water Bodies**.
    *   **Content**: Smaller patches of satellite imagery.

## 📄 Output
*   **Console**: displays execution time, speedup, and efficiency for each run.
*   **CSV Files**: `performance_results_1.csv` and `performance_results_2.csv` containing raw metrics.
*   **Images**: Processed images are saved in `output_images/` and `output_images2/`.
