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

template<size_t Size>
struct Payload
{
  char data[Size];
};

static void std_ctor_capture_8(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char* ()>([answer = Payload<8>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_8);

static void alt_ctor_capture_8(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<8>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_8);

static void std_ctor_capture_16(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char* ()>([answer = Payload<16>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_16);

static void alt_ctor_capture_16(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<16>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_16);

static void std_ctor_capture_32(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char* ()>([answer = Payload<32>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_32);

static void alt_ctor_capture_32(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<32>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_32);

static void std_ctor_capture_64(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char* ()>([answer = Payload<64>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_64);

static void alt_ctor_capture_64(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<64>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_64);

static void std_ctor_capture_128(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char* ()>([answer = Payload<128>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_128);

static void alt_ctor_capture_128(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<128>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_128);

static void std_ctor_capture_256(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::function<const char*()>([answer = Payload<256>{}]() { return answer.data; }));
  }
}
BENCHMARK(std_ctor_capture_256);

static void alt_ctor_capture_256(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(alt::function<const char* ()>([answer = Payload<256>{}]() { return answer.data; }));
  }
}
BENCHMARK(alt_ctor_capture_256);
