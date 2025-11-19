#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <omp.h>

// STB libraries per carregar i guardar imatges
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"

// Moduls amb les operacions morfologiques
#include "sequential.h"
#include "parallel.h"

namespace fs = std::filesystem;

const int GRAYSCALE_CHANNELS = 1;

// Converteix una imatge RGB a escala de grisos
std::vector<unsigned char> convert_to_grayscale(unsigned char* data, int width, int height, int n_channels) {
    std::vector<unsigned char> grayscale_data;
    grayscale_data.reserve(width * height);

    for (int i = 0; i < width * height; ++i) {
        if (n_channels >= 3) {
            unsigned char r = data[i * n_channels];
            unsigned char g = data[i * n_channels + 1];
            unsigned char b = data[i * n_channels + 2];
            // Formula estandard de luminositat
            unsigned char gray = static_cast<unsigned char>(std::round(0.299f * r + 0.587f * g + 0.114f * b));
            grayscale_data.push_back(gray);
        } else if (n_channels == 1) {
            grayscale_data.push_back(data[i]);
        }
    }
    return grayscale_data;
}


int main(int argc, char* argv[]) {

    // Configuracio de paths
    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / "input_images";
    const fs::path output_dir = project_root / "output_images";

    const int kernel_size = 21;

    int max_images = -1;
    int num_threads = omp_get_max_threads();

    // Llegir arguments de linia de comandes
    if (argc > 1) { max_images = std::stoi(argv[1]); }
    if (argc > 2) { num_threads = std::stoi(argv[2]); }

    omp_set_num_threads(num_threads);

    // Info inicial
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "  OPERACIO MORFOLOGICA: APERTURA" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Kernel: " << kernel_size << "x" << kernel_size << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Imatges: " << (max_images == -1 ? "TOTES" : std::to_string(max_images)) << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    // Comprovar directoris
    try {
        if (!fs::exists(output_dir)) { fs::create_directories(output_dir); }
        if (!fs::exists(input_dir)) {
            std::cerr << "ERROR: No existeix el directori d'entrada: " << input_dir << std::endl;
            return 1;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "ERROR filesystem: " << e.what() << std::endl;
        return 1;
    }

    // Variables per mesurar temps
    int images_processed = 0;
    double total_time_sequential_s = 0.0;
    double total_time_parallel_s = 0.0;

    // Processar cada imatge del directori
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

            // Convertir a grisos
            std::vector<unsigned char> gray_input = convert_to_grayscale(img_data, width, height, n_channels);
            stbi_image_free(img_data);

            std::cout << "\n> " << filename << " (" << width << "x" << height << ")" << std::endl;

            // Versio sequencial
            std::vector<unsigned char> seq_output;
            double start_seq = omp_get_wtime();
            Opening_Sequential(gray_input, seq_output, width, height, kernel_size);
            double end_seq = omp_get_wtime();
            double duration_seq_s = end_seq - start_seq;
            total_time_sequential_s += duration_seq_s;

            std::cout << "  [SEQ] " << duration_seq_s * 1000.0 << " ms" << std::endl;

            // Versio paral·lela
            std::vector<unsigned char> par_output;
            double start_par = omp_get_wtime();
            Opening_Parallel(gray_input, par_output, width, height, kernel_size);
            double end_par = omp_get_wtime();
            double duration_par_s = end_par - start_par;
            total_time_parallel_s += duration_par_s;

            std::cout << "  [PAR] " << duration_par_s * 1000.0 << " ms" << std::endl;

            // Speedup
            double speedup = duration_seq_s / duration_par_s;
            std::cout << "  [SPU] " << speedup << std::endl;

            // Guardar resultat
            std::string output_path = (output_dir / ("opened_" + filename)).string();
            stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS, par_output.data(), width * GRAYSCALE_CHANNELS);

            images_processed++;
        }
    }

    // Resum final
    std::cout << "\n------------------------------------------" << std::endl;
    std::cout << "  RESULTATS" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Imatges processades: " << images_processed << std::endl;
    if (images_processed > 0) {
        double overall_speedup = total_time_sequential_s / total_time_parallel_s;
        std::cout << "Temps total SEQ: " << total_time_sequential_s * 1000.0 << " ms" << std::endl;
        std::cout << "Temps total PAR: " << total_time_parallel_s * 1000.0 << " ms" << std::endl;
        std::cout << "SPEEDUP GLOBAL: " << overall_speedup << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;

    return 0;
}
