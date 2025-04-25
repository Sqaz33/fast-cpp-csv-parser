#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

void generate_large_csv(const std::string& filename, double target_size_gb = 1.0) {
    // Конфигурация генерации
    const std::vector<std::string> cities = {"New York", "London", "Tokyo", "Berlin", "Moscow"};
    const double target_size_bytes = target_size_gb * 1024 * 1024 * 1024;
    long long row_count = 0;
    
    // Создаем директорию, если нужно
    fs::path file_path(filename);
    if (!file_path.parent_path().empty()) {
        fs::create_directories(file_path.parent_path());
    }

    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> age_dist(18, 70);
    std::uniform_int_distribution<> city_dist(0, cities.size() - 1);
    std::uniform_real_distribution<> salary_dist(30000, 150000);

    // Открываем файл для записи
    std::ofstream csvfile(filename);
    if (!csvfile.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }

    // Записываем заголовок
    csvfile << "ID,Name,Age,City,Salary\n";

    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_print_time = start_time;

    while (true) {
        // Генерируем строку данных
        int age = age_dist(gen);
        std::string city = cities[city_dist(gen)];
        double salary = salary_dist(gen);

        // Форматируем строку CSV
        csvfile << row_count << ","
                << "User_" << row_count << ","
                << age << ","
                << city << ","
                << std::fixed << std::setprecision(2) << salary << "\n";

        row_count++;

        // Проверяем размер файла каждые 10000 строк
        if (row_count % 10000 == 0) {
            csvfile.flush(); // Обеспечиваем запись на диск
            auto current_size = fs::file_size(filename);
            
            // Выводим прогресс каждые 100000 строк или каждую секунду
            auto now = std::chrono::high_resolution_clock::now();
            if (row_count % 100000 == 0 || 
                std::chrono::duration_cast<std::chrono::seconds>(now - last_print_time).count() >= 1) {
                double current_size_gb = static_cast<double>(current_size) / (1024 * 1024 * 1024);
                std::cout << "Сгенерировано " << row_count << " строк (~" 
                          << current_size_gb << " ГБ)" << std::endl;
                last_print_time = now;
            }

            if (current_size >= target_size_bytes) {
                break;
            }
        }
    }

    csvfile.close();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    double final_size_gb = static_cast<double>(fs::file_size(filename)) / (1024 * 1024 * 1024);
    std::cout << "Файл '" << filename << "' создан.\n"
              << "Итоговый размер: " << final_size_gb << " ГБ\n"
              << "Всего строк: " << row_count << "\n"
              << "Время выполнения: " << duration.count() << " секунд" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " output_folder/filename.csv" << std::endl;
        return 1;
    }

    std::string output_path = argv[1];
    generate_large_csv(output_path);

    return 0;
}