#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"

#include "sequential.h"
#include "parallel.h"

namespace fs = std::filesystem;

const int GRAYSCALE_CHANNELS = 1;

// RGB to grayscale conversion
std::vector<unsigned char> convert_to_grayscale(unsigned char* data, int width, int height, int n_channels) {
    std::vector<unsigned char> grayscale_data;
    grayscale_data.reserve(width * height);

    for (int i = 0; i < width * height; ++i) {
        if (n_channels >= 3) {
            unsigned char r = data[i * n_channels];
            unsigned char g = data[i * n_channels + 1];
            unsigned char b = data[i * n_channels + 2];
            // standard luminosity formula
            unsigned char gray = static_cast<unsigned char>(std::round(0.299f * r + 0.587f * g + 0.114f * b));
            grayscale_data.push_back(gray);
        } else if (n_channels == 1) {
            grayscale_data.push_back(data[i]);
        }
    }
    return grayscale_data;
}


int main(int argc, char* argv[]) {

    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / "input_images";
    const fs::path output_dir = project_root / "output_images";

    const int kernel_size = 21;

    int max_images = -1;
    int num_threads = omp_get_max_threads();

    if (argc > 1) {
        max_images = std::stoi(argv[1]);
    }
    if (argc > 2) {
        num_threads = std::stoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    std::cout << "\n=== Morphological Opening ===" << std::endl;
    std::cout << "Kernel: " << kernel_size << "x" << kernel_size << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Processing images..." << std::endl;

    // make sure output directory exists
    try {
        if (!fs::exists(output_dir)) {
            fs::create_directories(output_dir);
        }
        if (!fs::exists(input_dir)) {
            std::cerr << "Error: input directory not found" << std::endl;
            return 1;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    int count = 0;
    double total_seq_time = 0.0;
    double total_par_time = 0.0;

    // process images
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (max_images != -1 && count >= max_images) {
            break;
        }

        if (entry.is_regular_file() &&
            (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {

            std::string input_path = entry.path().string();
            std::string filename = entry.path().filename().string();

            int width, height, channels;
            unsigned char* image_data = stbi_load(input_path.c_str(), &width, &height, &channels, 0);

            if (!image_data) continue;

            std::vector<unsigned char> gray_image = convert_to_grayscale(image_data, width, height, channels);
            stbi_image_free(image_data);

            std::cout << "\n" << filename << " (" << width << "x" << height << ")" << std::endl;

            // sequential version
            std::vector<unsigned char> result_seq;
            double start_seq = omp_get_wtime();
            Opening_Sequential(gray_image, result_seq, width, height, kernel_size);
            double end_seq = omp_get_wtime();
            double time_seq = end_seq - start_seq;
            total_seq_time += time_seq;

            // parallel version
            std::vector<unsigned char> result_par;
            double start_par = omp_get_wtime();
            Opening_Parallel(gray_image, result_par, width, height, kernel_size);
            double end_par = omp_get_wtime();
            double time_par = end_par - start_par;
            total_par_time += time_par;

            double speedup = time_seq / time_par;
            std::cout << "  Sequential: " << time_seq * 1000.0 << " ms" << std::endl;
            std::cout << "  Parallel:   " << time_par * 1000.0 << " ms" << std::endl;
            std::cout << "  Speedup:    " << speedup << "x" << std::endl;

            // save output
            std::string output_path = (output_dir / ("opened_" + filename)).string();
            stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS,
                          result_par.data(), width * GRAYSCALE_CHANNELS);

            count++;
        }
    }

    // print summary
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Total images processed: " << count << std::endl;
    if (count > 0) {
        std::cout << "Total sequential time: " << total_seq_time * 1000.0 << " ms" << std::endl;
        std::cout << "Total parallel time:   " << total_par_time * 1000.0 << " ms" << std::endl;
        std::cout << "Overall speedup:       " << total_seq_time / total_par_time << "x" << std::endl;
    }

    return 0;
}
