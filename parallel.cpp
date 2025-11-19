#include "parallel.h"
#include <omp.h>

// Parallel dilation using OpenMP
void Dilate_Parallel(const std::vector<unsigned char>& input,
                     std::vector<unsigned char>& output,
                     const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            unsigned char max_val = 0;

            for (int u = -kernel_radius; u <= kernel_radius; ++u) {
                for (int v = -kernel_radius; v <= kernel_radius; ++v) {
                    int ni = i + u;
                    int nj = j + v;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        unsigned char current_pixel = input[ni * width + nj];
                        if (current_pixel > max_val) {
                            max_val = current_pixel;
                        }
                    }
                }
            }
            output[i * width + j] = max_val;
        }
    }
}

// Parallel erosion using OpenMP
void Erode_Parallel(const std::vector<unsigned char>& input,
                    std::vector<unsigned char>& output,
                    const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            unsigned char min_val = 255;

            for (int u = -kernel_radius; u <= kernel_radius; ++u) {
                for (int v = -kernel_radius; v <= kernel_radius; ++v) {
                    int ni = i + u;
                    int nj = j + v;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        unsigned char current_pixel = input[ni * width + nj];
                        if (current_pixel < min_val) {
                            min_val = current_pixel;
                        }
                    }
                }
            }
            output[i * width + j] = min_val;
        }
    }
}

// Opening = erosion followed by dilation (both parallel)
void Opening_Parallel(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size) {

    std::vector<unsigned char> temp;

    Erode_Parallel(input, temp, width, height, kernel_size);
    Dilate_Parallel(temp, output, width, height, kernel_size);
}
