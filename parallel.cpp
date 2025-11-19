#include "parallel.h"
#include <omp.h>

// --- 1. DILATACIÓN PARALELA ---
void Dilate_Parallel(const std::vector<unsigned char>& input,
                     std::vector<unsigned char>& output,
                     const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

    // Directiva OpenMP: Paraleliza la iteración sobre los píxeles (Data Parallelism)
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            unsigned char max_val = 0; // Dilatación: MAX

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
            // Cada hilo escribe en una posición única de 'output'. No requiere sincronización.
            output[i * width + j] = max_val;
        }
    }
}

// --- 2. EROSIÓN PARALELA ---
void Erode_Parallel(const std::vector<unsigned char>& input,
                    std::vector<unsigned char>& output,
                    const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

    // Directiva OpenMP: Paraleliza la iteración sobre los píxeles (Data Parallelism)
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            unsigned char min_val = 255; // Erosión: MIN

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

// --- 3. APERTURA PARALELA (Erosión + Dilatación) ---
void Opening_Parallel(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size) {

    // Matriz temporal para la salida de la Erosión
    // Es crucial que esta matriz se pase a las funciones paralelas.
    std::vector<unsigned char> temp_eroded_output;

    // 1. Erosión Paralela
    Erode_Parallel(input, temp_eroded_output, width, height, kernel_size);

    // 2. Dilatación Paralela
    // Se ejecuta después de que Erode_Parallel haya terminado (sincronización implícita).
    Dilate_Parallel(temp_eroded_output, output, width, height, kernel_size);
}
