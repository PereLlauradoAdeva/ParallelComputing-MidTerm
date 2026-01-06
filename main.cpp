#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <omp.h>
#include <algorithm>

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

// Compare two images pixel by pixel
bool validate_results(const std::vector<unsigned char>& seq, const std::vector<unsigned char>& par) {
    if (seq.size() != par.size()) return false;
    for (size_t i = 0; i < seq.size(); ++i) {
        if (seq[i] != par[i]) return false;
    }
    return true;
}

void run_performance_test(const std::string& input_folder, const std::string& output_folder,
                          const std::string& csv_filename, int max_images, int kernel_size) {

    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / input_folder;
    const fs::path output_dir = project_root / output_folder;

    // Extended thread counts to test including oversubscription
    std::vector<int> thread_counts = {1, 2, 4, 8, 16, 32};
    std::vector<std::string> schedules = {"Static", "Dynamic"};

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
    csv_file << "Threads,Schedule,Sequential_Time_ms,Parallel_Time_ms,Speedup,Efficiency,Validation" << std::endl;

    // Get sequential baseline first
    std::cout << "\nCalculating sequential baseline..." << std::endl;
    double seq_baseline = 0.0;
    int count = 0;
    
    // Store sequential results for validation (only for the first few images to save memory/time)
    // Map filename -> result vector
    std::vector<std::pair<std::string, std::vector<unsigned char>>> seq_results_cache;
    const int MAX_VALIDATION_IMAGES = 5; 

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
            
            // Warm-up for sequential (1 run)
            std::vector<unsigned char> dummy;
            Opening_Sequential(gray_image, dummy, width, height, kernel_size);

            // Average over 10 runs
            double start = omp_get_wtime();
            for(int r=0; r<10; ++r) {
                 Opening_Sequential(gray_image, result_seq, width, height, kernel_size);
            }
            double end = omp_get_wtime();
            
            // Average time per image
            seq_baseline += (end - start) / 10.0;

            if (count < MAX_VALIDATION_IMAGES) {
                seq_results_cache.push_back({entry.path().filename().string(), result_seq});
            }

            count++;
        }
    }

    std::cout << "Sequential baseline (avg 10 runs): " << seq_baseline * 1000.0 << " ms (" << count << " images)" << std::endl;

    // Test each thread count
    for (int num_threads : thread_counts) {
        omp_set_num_threads(num_threads);

        for (const std::string& sched : schedules) {
            std::cout << "\nTesting with " << num_threads << " thread(s) [" << sched << "]..." << std::endl;

            double total_par_time = 0.0;
            int img_count = 0;
            bool all_valid = true;

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

                    // Execute based on schedule
                    // Warm-up (1 run)
                    std::vector<unsigned char> dummy;
                    if (sched == "Static") Opening_Parallel(gray_image, dummy, width, height, kernel_size);
                    else Opening_Parallel_Dynamic(gray_image, dummy, width, height, kernel_size);

                    // Average over 10 runs
                    double start = omp_get_wtime();
                    for(int r=0; r<10; ++r) {
                        if (sched == "Static") {
                            Opening_Parallel(gray_image, result_par, width, height, kernel_size);
                        } else {
                            Opening_Parallel_Dynamic(gray_image, result_par, width, height, kernel_size);
                        }
                    }
                    double end = omp_get_wtime();
                    total_par_time += (end - start) / 10.0;

                    // Validation
                    if (img_count < MAX_VALIDATION_IMAGES) {
                        // Find corresponding sequential result
                        for(const auto& item : seq_results_cache) {
                            if(item.first == filename) {
                                if(!validate_results(item.second, result_par)) {
                                    std::cout << "  X Validation failed for " << filename << std::endl;
                                    all_valid = false;
                                }
                                break;
                            }
                        }
                    }

                    // Save output image (only for first run of this configuration to avoid I/O blocking)
                    if (img_count == 0 || img_count == count - 1) { // Save first and last as sample
                        std::string name_without_ext = entry.path().stem().string();
                        std::string extension = entry.path().extension().string();
                        std::string output_filename = name_without_ext + "_" + std::to_string(num_threads) + "threads_" + sched + extension;
                        std::string output_path = (output_dir / output_filename).string();
                        stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS,
                                      result_par.data(), width * GRAYSCALE_CHANNELS);
                    }

                    img_count++;
                }
            }

            double speedup = seq_baseline / total_par_time;
            double efficiency = speedup / num_threads;

            std::cout << "  Parallel time: " << total_par_time * 1000.0 << " ms" << std::endl;
            std::cout << "  Speedup: " << speedup << "x" << std::endl;
            std::cout << "  Efficiency: " << efficiency * 100.0 << "%" << std::endl;
            std::cout << "  Validation: " << (all_valid ? "PASSED" : "FAILED") << std::endl;

            // Write to CSV
            csv_file << num_threads << ","
                     << sched << ","
                     << seq_baseline * 1000.0 << ","
                     << total_par_time * 1000.0 << ","
                     << speedup << ","
                     << efficiency << ","
                     << (all_valid ? "OK" : "FAIL") << std::endl;
        }
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
