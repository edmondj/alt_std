#include <benchmark/benchmark.h>
#include <functional>
#include <array>
#include <alt_std/functional.h>


template<template<typename> typename function>
static void ctor_fptr(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(function<void()>([]() {}));
  }
}
BENCHMARK(ctor_fptr<std::function>);
BENCHMARK(ctor_fptr<alt::function>);

template<template<typename> typename function, size_t size>
static void ctor_capture(benchmark::State& state) {
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
static void invoke(benchmark::State& state, function<int(int, int)> f)
{
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(f(1, 2));
  }
}
BENCHMARK_CAPTURE(invoke<std::function>, fptr, [](int l, int r) { return l + r; });
BENCHMARK_CAPTURE(invoke<alt::function>, fptr, [](int l, int r) { return l + r; });
BENCHMARK_CAPTURE(invoke<std::function>, sbo, [i = 42](int l, int r) { return l + r; });
BENCHMARK_CAPTURE(invoke<alt::function>, sbo, [i = 42](int l, int r) { return l + r; });
BENCHMARK_CAPTURE(invoke<std::function>, heavy, [i = std::array<char, 256>{}](int l, int r) { return l + r; });
BENCHMARK_CAPTURE(invoke<alt::function>, heavy, [i = std::array<char, 256>{}](int l, int r) { return l + r; });

template<template<typename> typename function>
static void move_fptr(benchmark::State& state, function<void()> init)
{
  for (auto _ : state)
  {
    state.PauseTiming();
    function<void()> src = init;
    function<void()> dst;
    state.ResumeTiming();
    dst = std::move(src);
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK_CAPTURE(move_fptr<std::function>, fptr, []() {});
BENCHMARK_CAPTURE(move_fptr<alt::function>, fptr, []() {});
BENCHMARK_CAPTURE(move_fptr<std::function>, sbo, [i = 42]() {});
BENCHMARK_CAPTURE(move_fptr<alt::function>, sbo, [i = 42]() {});
BENCHMARK_CAPTURE(move_fptr<std::function>, heavy, [i = std::array<char, 256>{}]() {});
BENCHMARK_CAPTURE(move_fptr<alt::function>, heavy, [i = std::array<char, 256>{}]() {});


template<template<typename> typename function>
static void copy_fptr(benchmark::State& state, function<void()> init)
{
  for (auto _ : state)
  {
    state.PauseTiming();
    function<void()> src = init;
    function<void()> dst;
    state.ResumeTiming();
    dst = src;
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK_CAPTURE(copy_fptr<std::function>, fptr, []() {});
BENCHMARK_CAPTURE(copy_fptr<alt::function>, fptr, []() {});
BENCHMARK_CAPTURE(copy_fptr<std::function>, sbo, [i = 42]() {});
BENCHMARK_CAPTURE(copy_fptr<alt::function>, sbo, [i = 42]() {});
BENCHMARK_CAPTURE(copy_fptr<std::function>, heavy, [i = std::array<char, 256>{}]() {});
BENCHMARK_CAPTURE(copy_fptr<alt::function>, heavy, [i = std::array<char, 256>{}]() {});
