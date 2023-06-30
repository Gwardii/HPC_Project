cmake_minimum_required(VERSION 3.12)
project(GeneticOptimization)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "lib/benchmark/build" "lib/oneapi-tbb/lib/cmake")
find_package(benchmark REQUIRED)
find_package(TBB REQUIRED COMPONENTS tbb)

# Add include directory to the search path
include_directories(include)

# Add source files to the project
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable
add_executable(GeneticOptimization ${SOURCES})
target_link_libraries(GeneticOptimization benchmark::benchmark)
target_link_libraries(GeneticOptimization TBB::tbb)
