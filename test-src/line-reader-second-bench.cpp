#include <benchmark/benchmark.h>
#include <thread>
#include <vector>
#include <atomic>
#include "csv.h"

static void stress_read_thread(int id, std::atomic<size_t>& total_lines, const std::string& path, size_t max_lines) {
    io::LineReader reader(path.c_str());
    char* line;
    size_t local_count = 0;
    while ((line = reader.next_line()) != nullptr && local_count < max_lines) {
        ++local_count;
    }
    total_lines += local_count;
}

static void BM_Stress_Multithreaded(benchmark::State& state) {
    const std::string file = "1gb_data.csv";
    const int thread_count = state.range(0);
    const size_t max_lines_per_thread = 1'000'000;

    for (auto _ : state) {
        std::atomic<size_t> total_lines{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back(stress_read_thread, i, std::ref(total_lines), file, max_lines_per_thread);
        }

        for (auto& t : threads) t.join();

        state.counters["Threads"] = thread_count;
        state.counters["TotalLines"] = total_lines.load();
    }
}

BENCHMARK(BM_Stress_Multithreaded)->Arg(2)->Arg(4)->Arg(8);
