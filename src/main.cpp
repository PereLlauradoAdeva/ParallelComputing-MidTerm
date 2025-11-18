/*
 * Morphological Image Processing - Sequential Version
 * Parallel Programming Project - Universitat degli Studi di Firenze
 *
 * Operations: Erosion, Dilation, Opening, Closing
 * Input: Grayscale images (PNG, JPG, BMP)
 *
 * Author: [Your Name]
 * Date: 2025
 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <algorithm>
#include <cmath>

// ============================================================================
// STB_IMAGE LIBRARY (Header-only)
// Download from: https://github.com/nothings/stb
// For now, we'll provide simple declarations
// ============================================================================

// stb_image.h - v2.28 - public domain image loader
#ifndef STB_IMAGE_H
#define STB_IMAGE_H
#ifdef __cplusplus
extern "C" {
#endif
unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
void stbi_image_free(void *retval_from_stbi_load);
#ifdef __cplusplus
}
#endif
#endif

// stb_image_write.h - v1.16 - public domain
#ifndef STB_IMAGE_WRITE_H
#define STB_IMAGE_WRITE_H
#ifdef __cplusplus
extern "C" {
#endif
int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
#ifdef __cplusplus
}
#endif
#endif

// ============================================================================
// IMAGE STRUCTURE
// ============================================================================

struct Image {
    int width;
    int height;
    int channels;
    std::vector<unsigned char> data;

    Image() : width(0), height(0), channels(0) {}

    Image(int w, int h, int c) : width(w), height(h), channels(c) {
        data.resize(w * h * c);
    }

    // Get pixel value at (x, y) for grayscale
    unsigned char getPixel(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return 0;
        return data[y * width + x];
    }

    // Set pixel value at (x, y) for grayscale
    void setPixel(int x, int y, unsigned char value) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[y * width + x] = value;
        }
    }

    bool isValid() const {
        return width > 0 && height > 0 && !data.empty();
    }
};

// ============================================================================
// STRUCTURING ELEMENT (KERNEL)
// ============================================================================

struct StructuringElement {
    int size;
    std::vector<int> offsets_x;
    std::vector<int> offsets_y;

    // Create a rectangular structuring element
    StructuringElement(int kernel_size) : size(kernel_size) {
        int half = kernel_size / 2;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                offsets_x.push_back(dx);
                offsets_y.push_back(dy);
            }
        }
    }

    // Create a circular structuring element
    static StructuringElement createCircular(int radius) {
        StructuringElement se(radius * 2 + 1);
        se.offsets_x.clear();
        se.offsets_y.clear();

        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx * dx + dy * dy <= radius * radius) {
                    se.offsets_x.push_back(dx);
                    se.offsets_y.push_back(dy);
                }
            }
        }
        return se;
    }
};

// ============================================================================
// IMAGE I/O FUNCTIONS
// ============================================================================

Image loadImage(const std::string& filename) {
    Image img;
    int width, height, channels;

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 1);

    if (!data) {
        std::cerr << "Error: Could not load image " << filename << std::endl;
        return img;
    }

    img.width = width;
    img.height = height;
    img.channels = 1;  // Force grayscale
    img.data.assign(data, data + (width * height));

    stbi_image_free(data);

    std::cout << "Loaded image: " << filename << " (" << width << "x" << height << ")" << std::endl;
    return img;
}

bool saveImage(const std::string& filename, const Image& img) {
    if (!img.isValid()) {
        std::cerr << "Error: Invalid image" << std::endl;
        return false;
    }

    int result = 0;

    // Determine format from extension
    if (filename.find(".png") != std::string::npos) {
        result = stbi_write_png(filename.c_str(), img.width, img.height, 1, img.data.data(), img.width);
    } else if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos) {
        result = stbi_write_jpg(filename.c_str(), img.width, img.height, 1, img.data.data(), 95);
    } else {
        // Default to PNG
        result = stbi_write_png(filename.c_str(), img.width, img.height, 1, img.data.data(), img.width);
    }

    if (result) {
        std::cout << "Saved image: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error: Could not save image " << filename << std::endl;
        return false;
    }
}

// ============================================================================
// MORPHOLOGICAL OPERATIONS (SEQUENTIAL)
// ============================================================================

/**
 * Erosion operation - reduces bright regions
 * For each pixel, takes the minimum value in the structuring element neighborhood
 */
Image erosion(const Image& input, const StructuringElement& se) {
    Image output(input.width, input.height, input.channels);

    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            unsigned char min_val = 255;

            // Check all positions in structuring element
            for (size_t i = 0; i < se.offsets_x.size(); i++) {
                int nx = x + se.offsets_x[i];
                int ny = y + se.offsets_y[i];

                unsigned char pixel_val = input.getPixel(nx, ny);
                min_val = std::min(min_val, pixel_val);
            }

            output.setPixel(x, y, min_val);
        }
    }

    return output;
}

/**
 * Dilation operation - expands bright regions
 * For each pixel, takes the maximum value in the structuring element neighborhood
 */
Image dilation(const Image& input, const StructuringElement& se) {
    Image output(input.width, input.height, input.channels);

    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            unsigned char max_val = 0;

            // Check all positions in structuring element
            for (size_t i = 0; i < se.offsets_x.size(); i++) {
                int nx = x + se.offsets_x[i];
                int ny = y + se.offsets_y[i];

                unsigned char pixel_val = input.getPixel(nx, ny);
                max_val = std::max(max_val, pixel_val);
            }

            output.setPixel(x, y, max_val);
        }
    }

    return output;
}

/**
 * Opening operation - erosion followed by dilation
 * Removes small bright spots (noise) while preserving larger structures
 */
Image opening(const Image& input, const StructuringElement& se) {
    Image eroded = erosion(input, se);
    Image result = dilation(eroded, se);
    return result;
}

/**
 * Closing operation - dilation followed by erosion
 * Fills small dark holes while preserving larger structures
 */
Image closing(const Image& input, const StructuringElement& se) {
    Image dilated = dilation(input, se);
    Image result = erosion(dilated, se);
    return result;
}

// ============================================================================
// TIMING UTILITIES
// ============================================================================

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;

public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double elapsed() {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time;
        return diff.count();
    }

    void printElapsed(const std::string& operation) {
        double time = elapsed();
        std::cout << operation << " completed in " << time << " seconds" << std::endl;
    }
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void printUsage(const char* program_name) {
    std::cout << "\nUsage: " << program_name << " <input_image> <output_image> <operation> <kernel_size>\n\n";
    std::cout << "Operations:\n";
    std::cout << "  erosion   - Erode the image (shrink bright regions)\n";
    std::cout << "  dilation  - Dilate the image (expand bright regions)\n";
    std::cout << "  opening   - Opening (erosion + dilation, remove noise)\n";
    std::cout << "  closing   - Closing (dilation + erosion, fill holes)\n\n";
    std::cout << "Kernel size: 3, 5, 7, 9, 11, etc. (odd numbers)\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << program_name << " input.png output.png erosion 5\n\n";
}

// ============================================================================
// MAIN PROGRAM
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "============================================\n";
    std::cout << "Morphological Image Processing - Sequential\n";
    std::cout << "============================================\n\n";

    // Check command line arguments
    if (argc != 5) {
        printUsage(argv[0]);
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::string operation = argv[3];
    int kernel_size = std::atoi(argv[4]);

    // Validate kernel size
    if (kernel_size < 3 || kernel_size % 2 == 0) {
        std::cerr << "Error: Kernel size must be an odd number >= 3\n";
        return 1;
    }

    // Load input image
    std::cout << "Loading image...\n";
    Image input = loadImage(input_file);
    if (!input.isValid()) {
        return 1;
    }

    // Create structuring element
    std::cout << "Creating structuring element (size: " << kernel_size << ")\n";
    StructuringElement se(kernel_size);

    // Perform operation
    Image output;
    Timer timer;

    std::cout << "Performing " << operation << "...\n";
    timer.start();

    if (operation == "erosion") {
        output = erosion(input, se);
    } else if (operation == "dilation") {
        output = dilation(input, se);
    } else if (operation == "opening") {
        output = opening(input, se);
    } else if (operation == "closing") {
        output = closing(input, se);
    } else {
        std::cerr << "Error: Unknown operation '" << operation << "'\n";
        printUsage(argv[0]);
        return 1;
    }

    timer.printElapsed(operation);

    // Calculate throughput
    long long total_pixels = (long long)input.width * input.height;
    double pixels_per_sec = total_pixels / timer.elapsed();
    std::cout << "Throughput: " << (pixels_per_sec / 1e6) << " Megapixels/second\n";

    // Save output image
    std::cout << "Saving result...\n";
    if (!saveImage(output_file, output)) {
        return 1;
    }

    std::cout << "\n✓ Processing completed successfully!\n";
    std::cout << "  Input:  " << input_file << " (" << input.width << "x" << input.height << ")\n";
    std::cout << "  Output: " << output_file << "\n";
    std::cout << "  Operation: " << operation << "\n";
    std::cout << "  Kernel: " << kernel_size << "x" << kernel_size << "\n\n";

    return 0;
}
