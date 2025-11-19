#include "sequential.h"

// --- 1. DILATACIÓN SECUENCIAL ---
void Dilate_Sequential(const std::vector<unsigned char>& input,
                       std::vector<unsigned char>& output,
                       const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

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

// --- 2. EROSIÓN SECUENCIAL ---
void Erode_Sequential(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size) {

    const int kernel_radius = kernel_size / 2;
    output.resize(width * height);

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

// --- 3. APERTURA SECUENCIAL (Erosión + Dilatación) ---
void Opening_Sequential(const std::vector<unsigned char>& input,
                        std::vector<unsigned char>& output,
                        const int width, const int height, const int kernel_size) {

    // Necesitamos una matriz temporal para la salida de la Erosión
    std::vector<unsigned char> temp_eroded_output;

    // 1. Erosión secuencial
    Erode_Sequential(input, temp_eroded_output, width, height, kernel_size);

    // 2. Dilatación secuencial
    Dilate_Sequential(temp_eroded_output, output, width, height, kernel_size);
}
