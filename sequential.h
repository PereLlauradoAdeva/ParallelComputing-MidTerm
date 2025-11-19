#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include <vector>

// --- OPERACIONES MORFOLÓGICAS SECUENCIALES ---

void Dilate_Sequential(const std::vector<unsigned char>& input,
                       std::vector<unsigned char>& output,
                       const int width, const int height, const int kernel_size);

void Erode_Sequential(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size);

void Opening_Sequential(const std::vector<unsigned char>& input,
                        std::vector<unsigned char>& output,
                        const int width, const int height, const int kernel_size);

#endif // SEQUENTIAL_H
