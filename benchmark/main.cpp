#include "noise.hpp"
#include <chrono>
#include <print>

static auto benchmark(const int n) {
    Noise noise(250, false, false);
    float sum = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        for (float x = 0; x < 1000.f; x++) {
            for (float y = 0; y < 1000.f; y++) {
                sum += noise.noise(22, { x, y }, 0.01f, 10.f, 300.f, 200.f);
            }
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("{} {}", sum, duration);
    return duration;
}

static auto benchmark_2(const int n) {
    MapGenSettings settings;
    for (ResourceType t = IRON; t < NB_RESOURCE_TYPE; ++t) {
        settings.frequencies[t] = 6.f;
        settings.sizes[t] = 6.f;
        settings.richness[t] = 6.f;
    }
    NoisePrecompute precompute(settings);
    NoiseCache cache;

    float sum = 0.f;

    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        auto patches = regular_patches(precompute, cache, i, { 0, 0 });
        for (const auto& patch : patches[1]) {
            sum += patch.radius;
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("{} {}", sum, duration);
    return duration;
}

static auto benchmark_3(const int n) {
    MapGenSettings settings;
    for (ResourceType t = IRON; t < NB_RESOURCE_TYPE; ++t) {
        settings.frequencies[t] = 6.f;
        settings.sizes[t] = 6.f;
        settings.richness[t] = 6.f;
    }
    NoisePrecompute precompute(settings);
    NoiseCache cache;
    
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        Noise noise(i, true, false);
        starter_patches(settings, precompute, noise, cache, i);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("{}", duration);
    return duration;
}

static auto benchmark_elevation(const int n) {
    Random rand(0);

    constexpr MapGenSettings settings;
    const NoisePrecompute precompute(settings);

    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        const auto seed = rand.random();
        const Noise noise(seed, true, true);

        for (int x = -1000; x <= 1000; ++x) {
            for (int y = -1000; y <= 1000; ++y) {
                noise.elevation(settings, precompute, {static_cast<float>(x), static_cast<float>(y)});;
            }
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("{}", duration);
    return duration;
}

static void run_bench(const std::function<std::chrono::milliseconds(int)>& func, const std::string& name,
    const int loops, const int iterations) {
    std::println("{}", name);

    std::chrono::milliseconds total_duration{0};
    for (int i = 0; i < loops; i++) {
        total_duration += func(iterations);
    }

    const auto avg = total_duration / loops;
    std::println("Average: {}", avg);
    std::println();
}

int main() {
    run_bench(benchmark_elevation, "Elevation", 10, 10);
    run_bench(benchmark, "1", 10, 500);
    run_bench(benchmark_2, "2", 10, 1000000);
    run_bench(benchmark_3, "3", 10, 100000);

    // 7.8s for 1000000
    // 6.7s => chunk_size = ceil(suggested_distance)
    // ~7-11s => penalties, quality and density giant lookup table
    // ~6.7-9s => penalties and density giant lookup table
    // 6.4s => only penalties lookup table
    // 6.1s => penalties lookup table and precomputed base density/quantity
    
    return 0;
}
