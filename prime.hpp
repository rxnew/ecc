/**
 * @file prime.hpp
 * @brief 素数判定, 素数生成クラスヘッダ
 * @date 2015.07.29
 */

#pragma once

#include <cassert>

class MillerRabin;
class PrimeGenerator;

/**
 * @brief ミラー-ラビン素数判定法クラス
 * @note コンパイル時計算対応
 */
class MillerRabin {
 private:
  static constexpr auto k = 100;
  template<class T>
  static constexpr auto pow(T n, T k, T q) -> T;
  template<class T>
  static constexpr auto exponent(T d) -> T;
  template<class T>
  static constexpr auto stride(T s) -> T;
  template<class T, class U>
  static constexpr auto test_impl_inner(T val, T y, U i) -> bool;
  template<class T>
  static constexpr auto test_impl(T val, T a, T s, T t, T y) -> bool;
  template<class T>
  static constexpr auto test(T val, T a, T t) -> bool;
 public:
  template<class T>
  static constexpr auto isPrime(T val) -> bool;
};

/**
 * @brief 素数生成クラス
 * @detail 指定した値以上で最小の素数を生成
 * @note コンパイル時計算対応
 */
class PrimeGenerator {
 private:
  template<class T>
  static constexpr auto create_impl(T odd) -> T;
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
constexpr auto MillerRabin::test_impl_inner(T val, T y, U i) -> bool {
  return !i ? true :
    (y == val - 1 ? false :
     MillerRabin::test_impl_inner(val, y * y % val, i - 1));
}

template<class T>
constexpr auto MillerRabin::test_impl(T val, T a, T s, T t, T y) -> bool {
  return a >= val ? true :
    (y != 1 && y != val - 1 &&
     MillerRabin::test_impl_inner(val, y, MillerRabin::k) ? false :
     MillerRabin::test_impl(val, a + s, s, t, MillerRabin::pow(a, t, val)));
}

template<class T>
constexpr auto MillerRabin::test(T val, T a, T t) -> bool {
  return MillerRabin::test_impl(val, a,
                                MillerRabin::stride(val / MillerRabin::k),
                                t, MillerRabin::pow(a, t, val));
}

template<class T>
constexpr auto MillerRabin::isPrime(T val) -> bool {
  return val == 1 || val & 1 == 0 ? false :
    (val == 2 ? true :
     MillerRabin::test(val, T(2), MillerRabin::exponent(val - 1)));
}

template<class T>
constexpr auto PrimeGenerator::create_impl(T odd) -> T {
  return MillerRabin::isPrime(odd) ? odd :
    PrimeGenerator::create_impl(odd + 2);
}

template<class T>
constexpr auto PrimeGenerator::create(T min) -> T {
  return min == 2 ? min :
    (min & 1 ?
     PrimeGenerator::create_impl(min) :
     PrimeGenerator::create_impl(min + 1));
}
