#include "alt_std/functional.h"
#include <type_traits>
#include <gtest/gtest.h>
#include <cstdio>
#include <array>

static size_t foo(int, char) { return 42; }

static auto get_lambda()
{
  return [](int, char) -> size_t { return 1337; };
}

struct move_only
{
  move_only() = default;
  move_only(move_only&&) = default;
  move_only(const move_only&) = delete;
  move_only& operator=(move_only&&) = default;
  move_only& operator=(const move_only&) = delete;
};

static auto get_move_only_lambda()
{
  return[mo = move_only{}](int, char)->size_t { return 0xDEADBEEF; };
}

static_assert(std::is_constructible_v<alt::move_only_function<size_t(int, char)>>);
static_assert(std::is_constructible_v<alt::move_only_function<size_t(int, char)>, decltype(&foo)>);
static_assert(std::is_constructible_v<alt::move_only_function<size_t(int, char)>, decltype(get_lambda())>);
static_assert(std::is_constructible_v<alt::move_only_function<size_t(int, char)>, decltype(get_move_only_lambda())>);

static_assert(std::is_constructible_v<alt::function<size_t(int, char)>>);
static_assert(std::is_constructible_v<alt::function<size_t(int, char)>, decltype(&foo)>);
static_assert(std::is_constructible_v<alt::function<size_t(int, char)>, decltype(get_lambda())>);
static_assert(!std::is_constructible_v<alt::function<size_t(int, char)>, decltype(get_move_only_lambda())>);
static_assert(std::is_constructible_v<alt::move_only_function<size_t(int, char)>, alt::function<size_t(int, char)>>);

TEST(functional, size)
{
  RecordProperty("sizeof(alt::function)", sizeof(alt::function<void()>));
  RecordProperty("sizeof(std::function)", sizeof(std::function<void()>));
  EXPECT_LT(sizeof(alt::function<void()>), sizeof(std::function<void()>));
}

TEST(functional, safety)
{
  static int count = 0;
  struct Ref
  {
    Ref() { ++count; }
    Ref(Ref&&) { ++count; }
    Ref(const Ref&) = delete;
    ~Ref() { --count; }
  };

  {
    alt::move_only_function<void(void)> func([ref = Ref{}]() {});
    EXPECT_EQ(count, 1);
  }
  EXPECT_EQ(count, 0);
}

TEST(functional, invokable_fptr)
{
  alt::move_only_function<size_t(int, char)> func = &foo;

  EXPECT_EQ(func(1, '*'), 42);

  EXPECT_EQ(alt::move_only_function<int(int, int)>([](int l, int r) { return l + r; })(3, 5), 8);
}

TEST(functional, invokable_obj)
{
  alt::move_only_function<int(int)> func = [i = 0, padding1 = size_t{}, padding2 = size_t{}](int j) mutable { return i += j; };

  EXPECT_EQ(func(1), 1);
  EXPECT_EQ(func(6), 7);
  EXPECT_EQ(func(23), 30);
}

TEST(functional, invokable_obj_sbo)
{
  alt::move_only_function<int(int)> func = [i = 0](int j) mutable { return i += j; };

  EXPECT_EQ(func(1), 1);
  EXPECT_EQ(func(6), 7);
  EXPECT_EQ(func(23), 30);
}

TEST(functional, invokable_fib)
{
  alt::move_only_function<size_t()> func = [i = size_t{ 1 }, j = size_t{ 1 }]() mutable
  {
    return i = std::exchange(j, j + i);
  };

  EXPECT_EQ(func(), 1);
  EXPECT_EQ(func(), 2);
  EXPECT_EQ(func(), 3);
  EXPECT_EQ(func(), 5);
  EXPECT_EQ(func(), 8);
}

TEST(functional, no_extra_copies_sbo)
{
  static int count = 0;
  struct Ref
  {
    Ref() { ++count; }
    Ref(Ref&&) { ++count; }
    Ref(const Ref&) { ++count; };
    ~Ref() { --count; }
  };

  alt::function<void(void)> func = [ref = Ref{}]() {};
  alt::function<void(void)> func2 = [ref = Ref{}]() {};

  EXPECT_EQ(count, 2);
  func2 = {};
  EXPECT_EQ(count, 1);
  func2 = func;
  EXPECT_EQ(count, 2);
}

TEST(functional, no_extra_copies)
{
  static int count = 0;
  struct Ref
  {
    Ref() { ++count; }
    Ref(Ref&&) { ++count; }
    Ref(const Ref&) { ++count; };
    ~Ref() { --count; }

    size_t payload[2];
  };

  alt::function<void(void)> func = [ref = Ref{}]() {};
  alt::function<void(void)> func2 = [ref = Ref{}]() {};

  EXPECT_EQ(count, 2);
  func2 = {};
  EXPECT_EQ(count, 1);
  func2 = func;
  EXPECT_EQ(count, 2);
}

TEST(functional, move)
{
  std::function<int()> f1;
  std::function<int()> f2;

  f1 = []() { return 42; };;
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = std::move(f1);
  EXPECT_FALSE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f2(), 42);

  f1 = [i = 42]() { return i; };
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = std::move(f1);
  EXPECT_FALSE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f2(), 42);

  f1 = [i = std::array<int, 128>{42}]() { return i[0]; };
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = std::move(f1);
  EXPECT_FALSE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f2(), 42);
}

TEST(functional, copy)
{
  std::function<int()> f1;
  std::function<int()> f2;

  f1 = []() { return 42; };;
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = f1;
  EXPECT_TRUE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f1(), 42);
  EXPECT_EQ(f2(), 42);

  f1 = [i = 42]() { return i; };
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = f1;
  EXPECT_TRUE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f1(), 42);
  EXPECT_EQ(f2(), 42);

  f1 = [i = std::array<int, 128>{42}]() { return i[0]; };
  f2 = {};
  EXPECT_TRUE(f1);
  EXPECT_FALSE(f2);
  f2 = f1;
  EXPECT_TRUE(f1);
  EXPECT_TRUE(f2);
  EXPECT_EQ(f1(), 42);
  EXPECT_EQ(f2(), 42);
}

TEST(functional, reassign)
{
  alt::move_only_function<int()> mo_func;

  EXPECT_FALSE(mo_func);
  mo_func = []() { return 42; };
  EXPECT_TRUE(mo_func);
  EXPECT_EQ(mo_func(), 42);
  mo_func = []() { return 1337; };
  EXPECT_TRUE(mo_func);
  EXPECT_EQ(mo_func(), 1337);

  alt::function<int()> func;

  EXPECT_FALSE(func);
  func = []() { return 42; };
  EXPECT_TRUE(func);
  EXPECT_EQ(func(), 42);
  func = []() { return 1337; };
  EXPECT_TRUE(func);
  EXPECT_EQ(func(), 1337);

  alt::function<int()> func2;

  func2 = func;
  EXPECT_TRUE(func);
  EXPECT_TRUE(func2);
  EXPECT_EQ(func(), 1337);
  EXPECT_EQ(func2(), 1337);
}

TEST(functional, heavy_move)
{
  alt::function<void()> src = [i = std::array<char, 256>{}]() {};
  alt::function<void()> dst;
  dst = std::move(src);
}

#if __cpp_exceptions

TEST(functional, exceptions)
{
  alt::function<void()> func;

  try
  {
    func();
  }
  catch (const alt::bad_function_call&)
  {
    SUCCEED();
    return;
  }
  FAIL();
}

#endif