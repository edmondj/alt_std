#include <benchmark/benchmark.h>
#include <functional>
#include <alt_std/functional.h>

static void std_ctor_fptr(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<void()>([]() {}));
  }
}
BENCHMARK(std_ctor_fptr);

static void alt_ctor_fptr(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<void()>([]() {}));
  }
}
BENCHMARK(alt_ctor_fptr);

static void std_ctor_sbo(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<size_t()>([answer = 42]() { return answer; }));
  }
}
BENCHMARK(std_ctor_sbo);

static void alt_ctor_sbo(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<size_t()>([answer = 42]() { return answer; }));
  }
}
BENCHMARK(alt_ctor_sbo);

struct HeavyObj
{
  char data[256];
};

static void std_ctor_big(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char*()>([answer = HeavyObj{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_big);

static void alt_ctor_big(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = HeavyObj{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_big);
