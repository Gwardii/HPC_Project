#ifndef GENETIC_OPTIMIZATION_HPP
#define GENETIC_OPTIMIZATION_HPP

#include <array>
#include <random>
#include <algorithm>
#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>
#include <tbb/enumerable_thread_specific.h>

struct Chromosome {
    double x;
    double fitness;
};

template <size_t PopulationSize, size_t NumGenerations, double MutationRate>
Chromosome geneticOptimizationSerial(double (*evaluate)(double)) {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    // Create an initial population of chromosomes
    std::array<Chromosome, PopulationSize> population;
    for (Chromosome& chromosome : population) {
        chromosome.x = dist(gen);
    }

    // Main genetic algorithm loop
    for (size_t generation = 0; generation < NumGenerations; ++generation) {
        // Evaluate the fitness of each chromosome in the population
        for (Chromosome& chromosome : population) {
            chromosome.fitness = evaluate(chromosome.x);
        }

        // Sort the population based on fitness (ascending order)
        std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });

        // Select the fittest individuals (parents) for reproduction
        constexpr size_t numParents = static_cast<size_t>(PopulationSize * 0.2);
        std::array<Chromosome, numParents> parents;
        std::copy_n(population.begin(), numParents, parents.begin());

        // Create the next generation through crossover and mutation
        std::array<Chromosome, PopulationSize> nextGeneration;
        std::copy(parents.begin(), parents.end(), nextGeneration.begin());
        size_t nextIndex = parents.size();
        while (nextIndex < PopulationSize) {
            const Chromosome& parent1 = parents[std::uniform_int_distribution<size_t>(0, parents.size() - 1)(gen)];
            const Chromosome& parent2 = parents[std::uniform_int_distribution<size_t>(0, parents.size() - 1)(gen)];
            Chromosome child;
            child.x = (parent1.x + parent2.x) / 2.0;  // Crossover
            if (std::uniform_real_distribution<double>(0.0, 1.0)(gen) < MutationRate) {
                child.x += std::uniform_real_distribution<double>(-1.0, 1.0)(gen);  // Mutation
            }
            nextGeneration[nextIndex++] = child;
        }

        // Replace the old population with the new generation
        population = nextGeneration;
    }

    return population[0];
}

template <size_t PopulationSize, size_t NumGenerations, double MutationRate>
Chromosome geneticOptimizationTBB(double (*evaluate)(double)) {
    tbb::enumerable_thread_specific<std::mt19937> gen([]() {
        std::random_device rd;
        return std::mt19937(rd());
    });
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    // Create an initial population of chromosomes
     std::array<Chromosome, PopulationSize> population;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, PopulationSize), [&](const tbb::blocked_range<size_t>& range) {
        auto& thread_gen = gen.local();
        for (size_t i = range.begin(); i < range.end(); ++i) {
            population[i].x = dist(thread_gen);
        }
    });

    // Main genetic algorithm loop
    for (size_t generation = 0; generation < NumGenerations; ++generation) {
        // Evaluate the fitness of each chromosome in the population in parallel
        tbb::parallel_for(tbb::blocked_range<size_t>(0, PopulationSize), [&](const tbb::blocked_range<size_t>& range) {
          for (size_t i = range.begin(); i < range.end(); ++i) {
              population[i].fitness = evaluate(population[i].x);
          }
        });

        // Sort the population based on fitness (ascending order)
        tbb::parallel_sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });

        // Select the fittest individuals (parents) for reproduction
        constexpr size_t numParents = static_cast<size_t>(PopulationSize * 0.2);
        std::array<Chromosome, numParents> parents;
        std::copy_n(population.begin(), numParents, parents.begin());

        // Create the next generation through crossover and mutation
        std::array<Chromosome, PopulationSize> nextGeneration;
        std::copy(parents.begin(), parents.end(), nextGeneration.begin());
        tbb::parallel_for(numParents, PopulationSize, [&](size_t i) {
            auto& thread_gen = gen.local();
            const Chromosome& parent1 = parents[std::uniform_int_distribution<size_t>(0, parents.size() - 1)(thread_gen)];
            const Chromosome& parent2 = parents[std::uniform_int_distribution<size_t>(0, parents.size() - 1)(thread_gen)];
            Chromosome child;
            child.x = (parent1.x + parent2.x) / 2.0;  // Crossover
            if (std::uniform_real_distribution<double>(0.0, 1.0)(thread_gen) < MutationRate) {
                child.x += std::uniform_real_distribution<double>(-1.0, 1.0)(thread_gen);  // Mutation
            }
            nextGeneration[i] = child;
        });

        // Replace the old population with the new generation
        population = nextGeneration;
    }

    return population[0];
}


#endif // GENETIC_OPTIMIZATION_HPP
