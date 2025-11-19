#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <omp.h>

// STB libraries for image loading/saving
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"

#include "sequential.h"
#include "parallel.h"

namespace fs = std::filesystem;

const int GRAYSCALE_CHANNELS = 1;

// Convert RGB image to grayscale
std::vector<unsigned char> convert_to_grayscale(unsigned char* data, int width, int height, int n_channels) {
    std::vector<unsigned char> grayscale_data;
    grayscale_data.reserve(width * height);

    for (int i = 0; i < width * height; ++i) {
        if (n_channels >= 3) {
            unsigned char r = data[i * n_channels];
            unsigned char g = data[i * n_channels + 1];
            unsigned char b = data[i * n_channels + 2];
            unsigned char gray = static_cast<unsigned char>(std::round(0.299f * r + 0.587f * g + 0.114f * b));
            grayscale_data.push_back(gray);
        } else if (n_channels == 1) {
            grayscale_data.push_back(data[i]);
        }
    }
    return grayscale_data;
}


int main(int argc, char* argv[]) {

    // Config
    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / "input_images";
    const fs::path output_dir = project_root / "output_images";

    const int kernel_size = 21;

    int max_images = -1;
    int num_threads = omp_get_max_threads();

    // Read command line args
    if (argc > 1) { max_images = std::stoi(argv[1]); }
    if (argc > 2) { num_threads = std::stoi(argv[2]); }

    omp_set_num_threads(num_threads);

    std::cout << "\n=== Morphological Opening ===" << std::endl;
    std::cout << "Kernel: " << kernel_size << "x" << kernel_size << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Images: " << (max_images == -1 ? "all" : std::to_string(max_images)) << std::endl;

    // Check directories
    try {
        if (!fs::exists(output_dir)) { fs::create_directories(output_dir); }
        if (!fs::exists(input_dir)) {
            std::cerr << "Error: input directory not found: " << input_dir << std::endl;
            return 1;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return 1;
    }

    int images_processed = 0;
    double total_seq = 0.0;
    double total_par = 0.0;

    // Process each image
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (images_processed == max_images && max_images != -1) {
            break;
        }

        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {

            const std::string input_path = entry.path().string();
            const std::string filename = entry.path().filename().string();

            int width, height, n_channels;
            unsigned char* img_data = stbi_load(input_path.c_str(), &width, &height, &n_channels, 0);

            if (!img_data) continue;

            std::vector<unsigned char> gray_input = convert_to_grayscale(img_data, width, height, n_channels);
            stbi_image_free(img_data);

            std::cout << "\n" << filename << " (" << width << "x" << height << ")" << std::endl;

            // Sequential version
            std::vector<unsigned char> seq_output;
            double t1 = omp_get_wtime();
            Opening_Sequential(gray_input, seq_output, width, height, kernel_size);
            double t2 = omp_get_wtime();
            double seq_time = t2 - t1;
            total_seq += seq_time;

            // Parallel version
            std::vector<unsigned char> par_output;
            double t3 = omp_get_wtime();
            Opening_Parallel(gray_input, par_output, width, height, kernel_size);
            double t4 = omp_get_wtime();
            double par_time = t4 - t3;
            total_par += par_time;

            double speedup = seq_time / par_time;
            std::cout << "  SEQ: " << seq_time * 1000.0 << " ms | PAR: " << par_time * 1000.0 << " ms | Speedup: " << speedup << std::endl;

            // Save result
            std::string output_path = (output_dir / ("opened_" + filename)).string();
            stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS, par_output.data(), width * GRAYSCALE_CHANNELS);

            images_processed++;
        }
    }

    // Final results
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Images: " << images_processed << std::endl;
    if (images_processed > 0) {
        std::cout << "Total SEQ: " << total_seq * 1000.0 << " ms" << std::endl;
        std::cout << "Total PAR: " << total_par * 1000.0 << " ms" << std::endl;
        std::cout << "Overall Speedup: " << total_seq / total_par << std::endl;
    }

    return 0;
}
