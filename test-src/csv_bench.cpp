#include <fstream>
#include <random>
#include <iomanip>

#include <benchmark/benchmark.h>

#include "csv.h"

// Фикстура для тестирования CSVReader
class CSVReaderFixture : public benchmark::Fixture {
protected:
    void CreateTestFile(const std::string& filename, size_t rows, bool quoted) {
        std::ofstream file(filename);
        file << "id,name,age,salary\n";
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> age_dist(18, 70);
        std::uniform_real_distribution<> salary_dist(30000, 150000);
        
        for (size_t i = 0; i < rows; ++i) {
            if (quoted) {
                file << i << ",\"User " << i << "\"," 
                     << age_dist(gen) << "," 
                     << std::fixed << std::setprecision(2) 
                     << salary_dist(gen) << "\n";
            } else {
                file << i << ",User " << i << "," 
                     << age_dist(gen) << "," 
                     << std::fixed << std::setprecision(2) 
                     << salary_dist(gen) << "\n";
            }
        }
    }
};

// Тест 1: Скорость парсинга большого CSV-файла
BENCHMARK_DEFINE_F(CSVReaderFixture, ParseLargeFile)(benchmark::State& state) {
    const std::string filename = "large_data.csv";
    CreateTestFile(filename, 10'000'000, false); // 10M строк
    
    for (auto _ : state) {
        io::CSVReader<4, io::trim_chars<' '>, io::no_quote_escape<','>> reader(filename);
        reader.read_header(io::ignore_extra_column, "id", "name", "age", "salary");
        
        int id, age;
        std::string name;
        double salary;
        
        while (reader.read_row(id, name, age, salary)) {
            benchmark::DoNotOptimize(id);
            benchmark::DoNotOptimize(name);
            benchmark::DoNotOptimize(age);
            benchmark::DoNotOptimize(salary);
        }
    }
    
    state.SetBytesProcessed(state.iterations() * 10'000'000 * 30); // ~30 bytes per row
    state.SetLabel("10M rows CSV");
}
BENCHMARK_REGISTER_F(CSVReaderFixture, ParseLargeFile)->Unit(benchmark::kMillisecond);

// Тест 2: Сравнение политик парсинга
BENCHMARK_DEFINE_F(CSVReaderFixture, PolicyOverhead)(benchmark::State& state) {
    const std::string filename = "test_data.csv";
    CreateTestFile(filename, 100'000, state.range(0)); // 100K строк
    
    for (auto _ : state) {
        if (state.range(0)) { // Quoted
            io::CSVReader<4, io::trim_chars<' '>, io::double_quote_escape<',','\"'>> reader(filename);
            reader.read_header(io::ignore_extra_column, "id", "name", "age", "salary");
            
            int id, age;
            std::string name;
            double salary;
            
            while (reader.read_row(id, name, age, salary)) {
                benchmark::DoNotOptimize(id);
                benchmark::DoNotOptimize(name);
                benchmark::DoNotOptimize(age);
                benchmark::DoNotOptimize(salary);
            }
        } else { // Not quoted
            io::CSVReader<4, io::trim_chars<' '>, io::no_quote_escape<','>> reader(filename);
            reader.read_header(io::ignore_extra_column, "id", "name", "age", "salary");
            
            int id, age;
            std::string name;
            double salary;
            
            while (reader.read_row(id, name, age, salary)) {
                benchmark::DoNotOptimize(id);
                benchmark::DoNotOptimize(name);
                benchmark::DoNotOptimize(age);
                benchmark::DoNotOptimize(salary);
            }
        }
    }
    
    state.SetItemsProcessed(state.iterations() * 100'000);
    state.SetLabel(state.range(0) ? "Quoted" : "Not quoted");
}
BENCHMARK_REGISTER_F(CSVReaderFixture, PolicyOverhead)
    ->Arg(0)->Arg(1) // Test both quoted and not quoted
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();