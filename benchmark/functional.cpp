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

template<template<typename> typename function>
void invoke_fptr(benchmark::State& state)
{
  function<int(int, int)> f = [](int l, int r) { return l + r; };
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(f(1, 2));
  }
}
BENCHMARK(invoke_fptr<std::function>);
BENCHMARK(invoke_fptr<alt::function>);

template<template<typename> typename function>
void invoke_sbo(benchmark::State& state)
{
  function<int(int, int)> f = [i = 42](int l, int r) { return l + r + i; };
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(f(1, 2));
  }
}
BENCHMARK(invoke_sbo<std::function>);
BENCHMARK(invoke_sbo<alt::function>);

template<template<typename> typename function>
void invoke_heavy(benchmark::State& state)
{
  char payload[256]{};
  function<int(int, int)> f = [payload](int l, int r) { return l + r; };
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(f(1, 2));
  }
}
BENCHMARK(invoke_heavy<std::function>);
BENCHMARK(invoke_heavy<alt::function>);
