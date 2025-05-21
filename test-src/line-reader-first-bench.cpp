#include <benchmark/benchmark.h>
#include <cstdio>
#include <memory>
#include "csv.h" // fast_cpp_csv_parser
using namespace std;

// Simple wrapper for LineReader performance
static void BM_LineReader_ReadLines(benchmark::State& state) {
    const string file_path = "1gb_data.csv";
    io::LineReader reader(file_path.c_str());
    size_t line_count = 0;

    for (auto _ : state) {
        reader.set_file_line(0); // Reset state
        char* line;
        line_count = 0;
        while ((line = reader.next_line()) != nullptr) {
            ++line_count;
            if (line_count >= state.range(0)) break; // Simulate partial load
        }
        state.PauseTiming();
        state.counters["Lines"] = line_count;
        state.ResumeTiming();
    }
}
BENCHMARK(BM_LineReader_ReadLines)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000);

BENCHMARK_MAIN();
