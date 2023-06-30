#include <benchmark/benchmark.h>
#include <cmath>
#include <tbb/global_control.h>
#include "geneticOptimization.hpp"
#include "testingFunctions.hpp"

const unsigned int numThreads = std::thread::hardware_concurrency()/2;
tbb::global_control c(tbb::global_control::max_allowed_parallelism, numThreads);

template <size_t populationSize, size_t numGenerations, double mutationRate>
static void BM_serial(benchmark::State& state) {
    // Run the benchmark for each state iteration
    for (auto _ : state) {
        // Perform genetic optimization with the specified parameters and fitness function
        auto bestChromosome = geneticOptimizationSerial<populationSize, numGenerations, mutationRate>(rastriginFunction);

        // Do not optimize away the result, as it may affect the benchmark
        benchmark::DoNotOptimize(bestChromosome);
    }
}

template <size_t populationSize, size_t numGenerations, double mutationRate>
static void BM_TBB(benchmark::State& state) {
    // Run the benchmark for each state iteration
    for (auto _ : state) {
        // Perform genetic optimization with the specified parameters and fitness function
        auto bestChromosome = geneticOptimizationTBB<populationSize, numGenerations, mutationRate>(rastriginFunction);

        // Do not optimize away the result, as it may affect the benchmark
        benchmark::DoNotOptimize(bestChromosome);
    }
}


// BENCHMARK(BM_serial<100, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<1000, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<10000, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<100, 1000, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<100, 10000, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<100, 100, 0.2>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_serial<100, 100, 0.4>)->Unit(benchmark::kMillisecond);

BENCHMARK(BM_serial<10000, 10000, 0.4>)->Unit(benchmark::kMillisecond);

// BENCHMARK(BM_TBB<100, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<1000, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<10000, 100, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<100, 1000, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<100, 10000, 0.1>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<100, 100, 0.2>)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_TBB<100, 100, 0.4>)->Unit(benchmark::kMillisecond);

BENCHMARK(BM_TBB<10000, 10000, 0.4>)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
