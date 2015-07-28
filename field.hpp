#pragma once

#include <iostream>
#include <iomanip>
#include <cassert>

template<class T, T q>
class F;

/* 正の剰余 */
template<class T>
inline auto mod(T n, T q) -> T {
  return (n %= q) < 0? n + q: n;
}

/* 法qでの逆元 */
template<class T>
auto invert(T n, T q) -> T {
  T r0 = q, r1 = n;
  T a0 = 1, a1 = 0;
  T b0 = 0, b1 = 1;
  while(r1 > 0) {
    T q1 = r0 / r1;
    T r2 = r0 % r1;
    T a2 = a0 - q1 * a1;
    T b2 = b0 - q1 * b1;
    r0 = r1, r1 = r2;
    a0 = a1, a1 = a2;
    b0 = b1, b1 = b2;
  }
  return (b0 + q) % q;
}

/* 繰り返し二乗法 */
template<class T, class U>
auto pow(const T& n, U k) -> T {
  if(k == 0) return T(1);
  T res = ::pow(n * n, k >> 1);
  if(k & 1) res = res * n;
  return res;
}

template<class T, class U, U q>
inline auto pow(const T& n, const F<U, q>& k) -> T {
  return ::pow(n, static_cast<U>(k));
}

template<class T>
T pow(T n, T k, T q) {
  if(k == 0) return 1;
  n = ::mod(n, q);
  T res = ::mod(::pow(n * n % q, k >> 1, q), q);
  if(k & 1) res = ::mod(res * n, q);
  return res;
}

/* 法qでの平方根 */
template<class T, T q>
auto sqrt(const F<T, q>& a) -> F<T, q> {
  using F = F<T, q>;
  F n(1);
  while(n.legendre() != -1) n++;
  T alpha, s = q - 1;
  for(alpha = 0; s % 2 == 0; alpha++) s >>= 1;
  F b = ::pow(n, s);
  F r = ::pow(a, (s + 1) >> 1);
  F j[alpha - 1];
  auto f = [&](const F& x, T i){
    return ::pow(::pow(x, 2) / a, ::pow(F(2), alpha - i - 2));};
  j[0] = F(f(r, 0) == 1? 0: 1);
  F j_sum = j[0];
  F b_tmp(1);
  for(T i = 1; i < alpha - 1; i++) {
    b_tmp *= ::pow(b, (::pow(F(2), i - 1) * j[i - 1]));
    j[i] = F(f(b_tmp * r, i) == 1? 0: 1);
    j_sum += (::pow(F(2), i) * j[i]);
  }
  return ::pow(b, j_sum) * r;
}

/* 有限体Fq */
template<class T, T q>
class F {
 protected:
  T val;
 public:
  F(): val(0) {}
  F(T val): val(::mod(val, q)) {}
  virtual ~F() {}
  auto operator==(const F& obj) const -> bool;
  auto operator==(T val) const -> bool;
  auto operator!=(const F& obj) const -> bool;
  auto operator!=(T val) const -> bool;
  auto operator+(const F& obj) const -> F;
  auto operator-(const F& obj) const -> F;
  auto operator*(const F& obj) const -> F;
  auto operator/(const F& obj) const -> F;
  auto operator-() const -> F;
  auto operator++(int) -> F;
  auto operator+=(const F& obj) -> F&;
  auto operator*=(const F& obj) -> F&;
  explicit operator T() const;
  inline static auto modulo() -> T {
    return q;
  }
  inline auto legendre() const -> int {
    int res = ::pow(*this, (q - 1) / 2).val;
    return res == q - 1? -1: res;
  }
  template<class U, U N>
  friend auto operator<<(std::ostream& os, const F<U, N>& obj)
    -> std::ostream&;
};

template<class T, T q>
auto F<T, q>::operator==(const F& obj) const -> bool {
  return this->val == obj.val;
}

template<class T, T q>
auto F<T, q>::operator==(T val) const -> bool {
  return this->val == ::mod(val, q);
}

template<class T, T q>
auto F<T, q>::operator!=(const F& obj) const -> bool {
  return !(*this == obj);
}

template<class T, T q>
auto F<T, q>::operator!=(T val) const -> bool {
  return !(*this == val);
}

template<class T, T q>
auto F<T, q>::operator+(const F& obj) const -> F<T, q> {
  return F<T, q>(::mod(this->val + obj.val, q));
}

template<class T, T q>
auto F<T, q>::operator-(const F& obj) const -> F<T, q> {
  return F<T, q>(::mod(this->val - obj.val, q));
}

template<class T, T q>
auto F<T, q>::operator*(const F& obj) const -> F<T, q> {
  return F<T, q>(::mod(this->val * obj.val, q));
}

template<class T, T q>
auto F<T, q>::operator/(const F& obj) const -> F<T, q> {
  return F<T, q>(::mod(this->val * ::invert(obj.val, q), q));
}

template<class T, T q>
auto F<T, q>::operator-() const -> F<T, q> {
  return F<T, q>(::mod(-this->val, q));
}

template<class T, T q>
auto F<T, q>::operator++(int) -> F<T, q> {
  F<T, q> tmp(this->val);
  this->val = ::mod(this->val + 1, q);
  return tmp;
}

template<class T, T q>
auto F<T, q>::operator+=(const F& obj) -> F<T, q>& {
  return *this = *this + obj;
}

template<class T, T q>
auto F<T, q>::operator*=(const F& obj) -> F<T, q>& {
  return *this = *this * obj;
}

template<class T, T q>
F<T, q>::operator T() const {
  return this->val;
}

template<class T, T q>
auto operator<<(std::ostream& os, const F<T, q>& obj) -> std::ostream& {
  return os << obj.val;
}
