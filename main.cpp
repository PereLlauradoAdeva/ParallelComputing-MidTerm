#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <omp.h> // ¡Añadido para OpenMP!

// --- Definiciones para STB_IMAGE ---
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h" 

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h" 

namespace fs = std::filesystem;
using namespace std::chrono;

// --- CONSTANTES Y UTILIDADES ---

const int GRAYSCALE_CHANNELS = 1;

/**
 * @brief Convierte una imagen a escala de grises.
 */
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


// ####################################################################
// ## OPERACIONES MORFOLÓGICAS BÁSICAS (DILATACIÓN y EROSIÓN)
// ####################################################################

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


// --- 3. DILATACIÓN SECUENCIAL (para referencia del Speedup) ---
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

// --- 4. EROSIÓN SECUENCIAL (para referencia del Speedup) ---
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


// ####################################################################
// ## OPERACIÓN COMPUESTA: APERTURA (Erosión seguida de Dilatación)
// ####################################################################

/**
 * @brief Implementa la Apertura secuencial (Erode_Sequential + Dilate_Sequential).
 */
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

/**
 * @brief Implementa la Apertura paralela (Erode_Parallel + Dilate_Parallel).
 */
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


// ####################################################################
// ## LÓGICA PRINCIPAL (MEDICIÓN Y EJECUCIÓN)
// ####################################################################

int main(int argc, char* argv[]) {
    
    // --- PARÁMETROS DE CONFIGURACIÓN ---
    const fs::path project_root = "C:\\Users\\Lenovo\\Desktop\\UNIFI\\Parallel\\ProjectMidTermDefinitiu";
    const fs::path input_dir = project_root / "input_images";
    const fs::path output_dir = project_root / "output_images";
    
    const int kernel_size = 9; 

    int max_images = -1; 
    int num_threads = omp_get_max_threads(); 

    // 1. Lectura de argumentos: <./run_executable> <max_images> <num_threads>
    if (argc > 1) { max_images = std::stoi(argv[1]); }
    if (argc > 2) { num_threads = std::stoi(argv[2]); }
    
    // 2. Configurar el número de hilos de OpenMP
    omp_set_num_threads(num_threads);

    // --- ENCABEZADO DE INFORME ---
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "  PREPROCESAMIENTO MORFOLÓGICO (APERTURA)" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Operación: Apertura (Erosión + Dilatación)" << std::endl;
    std::cout << "Kernel Size: " << kernel_size << "x" << kernel_size << std::endl;
    std::cout << "Hilos (OMP_NUM_THREADS): " << num_threads << std::endl;
    std::cout << "Imágenes a procesar: " << (max_images == -1 ? "TODAS" : std::to_string(max_images)) << std::endl;
    std::cout << "------------------------------------------" << std::endl;


    // --- CONFIGURACIÓN DE DIRECTORIOS ---
    try {
        if (!fs::exists(output_dir)) { fs::create_directories(output_dir); }
        if (!fs::exists(input_dir)) { 
            std::cerr << "ERROR: El directorio de entrada NO existe: " << input_dir << std::endl; 
            return 1; 
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "ERROR de sistema de archivos: " << e.what() << std::endl;
        return 1;
    }

    // --- BUCLE DE PROCESAMIENTO ---
    int images_processed = 0;
    
    double total_time_sequential_s = 0.0;
    double total_time_parallel_s = 0.0;

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

            // Preparación: Conversión a grises
            std::vector<unsigned char> gray_input = convert_to_grayscale(img_data, width, height, n_channels);
            stbi_image_free(img_data); 
            
            std::cout << "\n> Procesando: " << filename << " (" << width << "x" << height << ")" << std::endl;

            // --- CÁLCULO SECUENCIAL Y MEDICIÓN ---
            std::vector<unsigned char> seq_output;
            double start_seq = omp_get_wtime();
            
            Opening_Sequential(gray_input, seq_output, width, height, kernel_size); 
            
            double end_seq = omp_get_wtime();
            double duration_seq_s = end_seq - start_seq;
            total_time_sequential_s += duration_seq_s;
            
            std::cout << "  [SEQ] Tiempo de Apertura: " << duration_seq_s * 1000.0 << " ms" << std::endl;


            // --- CÁLCULO PARALELO Y MEDICIÓN ---
            std::vector<unsigned char> par_output;
            double start_par = omp_get_wtime();
            
            Opening_Parallel(gray_input, par_output, width, height, kernel_size); 
            
            double end_par = omp_get_wtime();
            double duration_par_s = end_par - start_par;
            total_time_parallel_s += duration_par_s;
            
            std::cout << "  [PAR] Tiempo de Apertura: " << duration_par_s * 1000.0 << " ms" << std::endl;
            
            // Cálculo del Speedup por imagen
            double speedup = duration_seq_s / duration_par_s;
            std::cout << "  [SPU] Speedup (por imagen): " << speedup << std::endl;


            // --- GUARDAR RESULTADOS ---
            std::string output_path = (output_dir / ("opened_PAR_" + filename)).string();
            stbi_write_png(output_path.c_str(), width, height, GRAYSCALE_CHANNELS, par_output.data(), width * GRAYSCALE_CHANNELS);
            
            images_processed++;
        }
    }

    // --- RESUMEN FINAL ---
    std::cout << "\n------------------------------------------" << std::endl;
    std::cout << "          RESUMEN DE RENDIMIENTO          " << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Total de imágenes procesadas: " << images_processed << std::endl;
    if (images_processed > 0) {
        double overall_speedup = total_time_sequential_s / total_time_parallel_s;
        std::cout << "Tiempo total secuencial (computación): " << total_time_sequential_s * 1000.0 << " ms" << std::endl;
        std::cout << "Tiempo total paralelo (computación):   " << total_time_parallel_s * 1000.0 << " ms" << std::endl;
        std::cout << "SPEEDUP GLOBAL: " << overall_speedup << std::endl;
    }
    std::cout << "--- FIN ---" << std::endl;

    return 0;
}