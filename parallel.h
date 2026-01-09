#ifndef PARALLEL_H
#define PARALLEL_H

#include <vector>

// Parallel morphology operations (OpenMP)

void Dilate_Parallel(const std::vector<unsigned char>& input,
                     std::vector<unsigned char>& output,
                     const int width, const int height, const int kernel_size);

void Erode_Parallel(const std::vector<unsigned char>& input,
                    std::vector<unsigned char>& output,
                    const int width, const int height, const int kernel_size);

void Opening_Parallel(const std::vector<unsigned char>& input,
                      std::vector<unsigned char>& output,
                      const int width, const int height, const int kernel_size);

// Dynamic scheduling

void Dilate_Parallel_Dynamic(const std::vector<unsigned char>& input,
                             std::vector<unsigned char>& output,
                             const int width, const int height, const int kernel_size);

void Erode_Parallel_Dynamic(const std::vector<unsigned char>& input,
                            std::vector<unsigned char>& output,
                            const int width, const int height, const int kernel_size);

void Opening_Parallel_Dynamic(const std::vector<unsigned char>& input,
                              std::vector<unsigned char>& output,
                              const int width, const int height, const int kernel_size);

#endif // PARALLEL_H
