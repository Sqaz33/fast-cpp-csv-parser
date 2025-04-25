#include <fstream>
#include <cstdio>
#include <iostream>

#include <benchmark/benchmark.h>

#include "csv.h"

// Фикстура для тестирования LineReader
class LineReaderFixture : public benchmark::Fixture {
public:
    void SetUp(const benchmark::State& state) override {
        // Создаем тестовый файл 1GB (если еще не создан)
        if (!std::ifstream("1gb_data.csv").good()) {
            std::cerr << "Сначала создайте тестовый файл 1gb_data.csv\n";
            exit(1);
        }
    }

    void TearDown(const benchmark::State&) override {}
};

// Тест 1: Скорость чтения большого файла
BENCHMARK_DEFINE_F(LineReaderFixture, ReadLargeFile)(benchmark::State& state) {
    for (auto _ : state) {
        io::LineReader reader("1gb_data.csv");
        while (reader.next_line() != nullptr) {
            // Просто читаем все строки
        }
    }
    
    // Устанавливаем метрики
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * 1024 * 1024 * 1024);
    state.SetLabel("1GB CSV file");
}
BENCHMARK_REGISTER_F(LineReaderFixture, ReadLargeFile)->Unit(benchmark::kMillisecond);

// Тест 2: Накладные расходы next_line()
BENCHMARK_DEFINE_F(LineReaderFixture, NextLineOverhead)(benchmark::State& state) {
    // Создаем небольшой тестовый файл (1000 строк)
    {
        std::ofstream tmp("test_small.csv");
        for (int i = 0; i < 1000; ++i) {
            tmp << "Line " << i << "\n";
        }
    }

    for (auto _ : state) {
        io::LineReader reader("test_small.csv");
        while (reader.next_line() != nullptr) {
            benchmark::DoNotOptimize(reader.next_line());
        }
    }
    
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 1000);
    state.SetLabel("1000 lines file");
}
BENCHMARK_REGISTER_F(LineReaderFixture, NextLineOverhead)->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();

