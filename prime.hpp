#pragma once

#include <random>
#include <cassert>

class MillerRabin;
class PrimeGenerator;

class MillerRabin {
 private:
  constexpr static auto k = 100;
  template<class T>
  constexpr static auto pow(T n, T k, T q) -> T;
  template<class T>
  constexpr static auto exponent(T d) -> T;
  template<class T>
  constexpr static auto stride(T s) -> T;
  template<class T, class U>
  constexpr static auto loop(T val, T y, U i) -> bool;
  template<class T>
  constexpr static auto test(T val, T a, T s, T t, T y) -> bool;
  template<class T>
  constexpr static auto set(T val, T a, T t) -> bool;
 public:
  template<class T>
  constexpr static auto isPrime(T val) -> bool;
};

class PrimeGenerator {
 private:
  template<class T>
  static constexpr auto test(T odd) -> T;
 public:
  template<class T>
  static constexpr auto create(T min) -> T;
};

template<class T>
constexpr auto MillerRabin::pow(T n, T k, T q) -> T {
  return !k ? T(1) :
    (k & 1 ?
     MillerRabin::pow(n * n % q, k >> 1, q) * n % q :
     MillerRabin::pow(n * n % q, k >> 1, q));
}

template<class T>
constexpr auto MillerRabin::exponent(T d) -> T {
  return d & 1 ? d : MillerRabin::exponent(d >> 1);
}

template<class T>
constexpr auto MillerRabin::stride(T s) -> T {
  return s ? s : 1;
}

template<class T, class U>
constexpr auto MillerRabin::loop(T val, T y, U i) -> bool {
  return !i ? true :
    (y == val - 1 ? false :
     MillerRabin::loop(val, y * y % val, i - 1));
}

template<class T>
constexpr auto MillerRabin::test(T val, T a, T s, T t, T y) -> bool {
  return a >= val ? true :
    (y != 1 && y != val - 1 &&
     MillerRabin::loop(val, y, MillerRabin::k) ? false :
     MillerRabin::test(val, a + s, s, t, MillerRabin::pow(a, t, val)));
}

template<class T>
constexpr auto MillerRabin::set(T val, T a, T t) -> bool {
  return MillerRabin::test(val, a, MillerRabin::stride(val / MillerRabin::k),
                           t, MillerRabin::pow(a, t, val));
}

template<class T>
constexpr auto MillerRabin::isPrime(T val) -> bool {
  return val == 1 || val & 1 == 0 ? false :
    (val == 2 ? true :
     MillerRabin::set(val, T(2), MillerRabin::exponent(val - 1)));
}

template<class T>
constexpr auto PrimeGenerator::test(T odd) -> T {
  return MillerRabin::isPrime(odd) ? odd : PrimeGenerator::test(odd + 2);
}

template<class T>
constexpr auto PrimeGenerator::create(T min) -> T {
  return min == 2 ? min :
    (min & 1 ?
     PrimeGenerator::test(min) :
     PrimeGenerator::test(min + 1));
}
