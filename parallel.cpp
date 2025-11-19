#include "parallel.h"
#include <omp.h>

// Dilatacio paral·lela amb OpenMP
void Dilate_Parallel(const std::vector<unsigned char>& input,
                     std::vector<unsigned char>& output,
                     const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

    // Paral·lelitzem els dos bucles externs
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
            // Cada thread escriu a una posicio diferent, no cal sincronitzar
            output[i * width + j] = max_val;
        }
    }
}

// Erosio paral·lela amb OpenMP
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

// Apertura = erosio + dilatacio (ambdues paral·leles)
void Opening_Parallel(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size) {

    std::vector<unsigned char> temp;

    // Primer erosio
    Erode_Parallel(input, temp, width, height, kernel_size);

    // Despres dilatacio (espera implicita a que acabi l'erosio)
    Dilate_Parallel(temp, output, width, height, kernel_size);
}
