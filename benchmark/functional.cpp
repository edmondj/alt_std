#include <benchmark/benchmark.h>
#include <functional>
#include <alt_std/functional.h>


template<template<typename> typename function>
void ctor_fptr(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(function<void()>([]() {}));
  }
}
BENCHMARK(ctor_fptr<std::function>);
BENCHMARK(ctor_fptr<alt::function>);

template<template<typename> typename function, size_t size>
void ctor_capture(benchmark::State& state) {
  struct payload
  {
    char data[size];
  };

  for (auto _ : state) {
    benchmark::DoNotOptimize(function<const char* ()>([answer = payload{}]() { return answer.data; }));
  }
}
BENCHMARK(ctor_capture<std::function, 8>);
BENCHMARK(ctor_capture<alt::function, 8>);
BENCHMARK(ctor_capture<std::function, 16>);
BENCHMARK(ctor_capture<alt::function, 16>);
BENCHMARK(ctor_capture<std::function, 32>);
BENCHMARK(ctor_capture<alt::function, 32>);
BENCHMARK(ctor_capture<std::function, 64>);
BENCHMARK(ctor_capture<alt::function, 64>);
BENCHMARK(ctor_capture<std::function, 128>);
BENCHMARK(ctor_capture<alt::function, 128>);
BENCHMARK(ctor_capture<std::function, 256>);
BENCHMARK(ctor_capture<alt::function, 256>);
