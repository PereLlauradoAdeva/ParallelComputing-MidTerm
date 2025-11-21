#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
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
            unsigned char gray = static_cast<unsigned char>(std::round(0.299f * r + 0.587f * g + 0.114f * b));
            grayscale_data.push_back(gray);
        } else if (n_channels == 1) {
            grayscale_data.push_back(data[i]);
        }
    }
    return grayscale_data;
}

void run_performance_test(const std::string& input_folder, const std::string& output_folder,
                          const std::string& csv_filename, int max_images, int kernel_size) {

    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / input_folder;
    const fs::path output_dir = project_root / output_folder;

    // Thread counts to test
    std::vector<int> thread_counts = {1, 2, 4, 8, 12, 16};

    // make sure directories exist
    try {
        if (!fs::exists(output_dir)) {
            fs::create_directories(output_dir);
        }
        if (!fs::exists(input_dir)) {
            std::cerr << "Error: input directory not found: " << input_dir << std::endl;
            return;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

    std::cout << "\n=== Testing " << input_folder << " ===" << std::endl;
    std::cout << "Kernel: " << kernel_size << "x" << kernel_size << std::endl;
    std::cout << "Images: " << (max_images == -1 ? "all" : std::to_string(max_images)) << std::endl;

    // Open CSV file for results
    std::ofstream csv_file(csv_filename);
    csv_file << "Threads,Sequential_Time_ms,Parallel_Time_ms,Speedup,Efficiency" << std::endl;

    // Get sequential baseline first
    std::cout << "\nCalculating sequential baseline..." << std::endl;
    double seq_baseline = 0.0;
    int count = 0;

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (max_images != -1 && count >= max_images) break;

        if (entry.is_regular_file() &&
            (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {

            std::string input_path = entry.path().string();
            int width, height, channels;
            unsigned char* image_data = stbi_load(input_path.c_str(), &width, &height, &channels, 0);
            if (!image_data) continue;

            std::vector<unsigned char> gray_image = convert_to_grayscale(image_data, width, height, channels);
            stbi_image_free(image_data);

            std::vector<unsigned char> result_seq;
            double start = omp_get_wtime();
            Opening_Sequential(gray_image, result_seq, width, height, kernel_size);
            double end = omp_get_wtime();
            seq_baseline += (end - start);

            count++;
        }
    }

    std::cout << "Sequential baseline: " << seq_baseline * 1000.0 << " ms (" << count << " images)" << std::endl;

    // Test each thread count
    for (int num_threads : thread_counts) {
        omp_set_num_threads(num_threads);

        std::cout << "\nTesting with " << num_threads << " thread(s)..." << std::endl;

        double total_par_time = 0.0;
        int img_count = 0;

        for (const auto& entry : fs::directory_iterator(input_dir)) {
            if (max_images != -1 && img_count >= max_images) break;

            if (entry.is_regular_file() &&
                (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {

                std::string input_path = entry.path().string();
                std::string filename = entry.path().filename().string();
                int width, height, channels;
                unsigned char* image_data = stbi_load(input_path.c_str(), &width, &height, &channels, 0);
                if (!image_data) continue;

                std::vector<unsigned char> gray_image = convert_to_grayscale(image_data, width, height, channels);
                stbi_image_free(image_data);

                std::vector<unsigned char> result_par;
                double start = omp_get_wtime();
                Opening_Parallel(gray_image, result_par, width, height, kernel_size);
                double end = omp_get_wtime();
                total_par_time += (end - start);

                // Save output image for each thread count
                std::string name_without_ext = entry.path().stem().string();
                std::string extension = entry.path().extension().string();
                std::string output_filename = name_without_ext + "_" + std::to_string(num_threads) + "threads" + extension;
                std::string output_path = (output_dir / output_filename).string();
                stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS,
                              result_par.data(), width * GRAYSCALE_CHANNELS);

                img_count++;
            }
        }

        double speedup = seq_baseline / total_par_time;
        double efficiency = speedup / num_threads;

        std::cout << "  Parallel time: " << total_par_time * 1000.0 << " ms" << std::endl;
        std::cout << "  Speedup: " << speedup << "x" << std::endl;
        std::cout << "  Efficiency: " << efficiency * 100.0 << "%" << std::endl;

        // Write to CSV
        csv_file << num_threads << ","
                 << seq_baseline * 1000.0 << ","
                 << total_par_time * 1000.0 << ","
                 << speedup << ","
                 << efficiency << std::endl;
    }

    csv_file.close();
    std::cout << "\nResults saved to " << csv_filename << std::endl;
}


int main(int argc, char* argv[]) {

    const int kernel_size = 9;

    int max_images1 = -1;
    int max_images2 = -1;

    if (argc > 1) {
        max_images1 = std::stoi(argv[1]);
    }
    if (argc > 2) {
        max_images2 = std::stoi(argv[2]);
    }

    std::cout << "=== Thread Performance Test ===" << std::endl;
    std::cout << "Test 1: input_images (max: " << (max_images1 == -1 ? "all" : std::to_string(max_images1)) << ")" << std::endl;
    std::cout << "Test 2: input_images2 (max: " << (max_images2 == -1 ? "all" : std::to_string(max_images2)) << ")" << std::endl;

    // Run test 1
    run_performance_test("input_images", "output_images", "performance_results_1.csv", max_images1, kernel_size);

    // Run test 2
    run_performance_test("input_images2", "output_images2", "performance_results_2.csv", max_images2, kernel_size);

    std::cout << "\n=== All tests completed ===" << std::endl;
    std::cout << "Results saved to performance_results_1.csv and performance_results_2.csv" << std::endl;

    return 0;
}
