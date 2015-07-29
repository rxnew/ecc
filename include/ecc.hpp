/**
 * @file ecc.hpp
 * @brief 楕円曲線暗号クラスヘッダ
 * @date 2015.07.29
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <limits>
#include <random>
#include <cassert>
#include "field.hpp"
#include "prime.hpp"

template<class F>
class FInf;

template<class F>
class EccPoint;

template<class F>
class Ecc;

template<class F>
class EccUser;

template<class F>
class Ecdh;

/**
 * @brief 有限体クラスの無限大
 * @detail 無限遠点の値として例外的に使用する
 */
template<class F>
class FInf : public F {
 public:
  FInf();
};

/**
 * @brief 楕円曲線上の点
 * @note 無限遠点の場合はFInfクラスを用いる
 */
template<class F>
class EccPoint {
 private:
  F x, y;
 public:
  EccPoint() : x(FInf<F>()), y(FInf<F>()) {}
  template<class T>
  EccPoint(T x, T y) : x(F(x)), y(F(y)) {}
  EccPoint(const F& x, const F& y) : x(x), y(y) {}
  auto operator==(const EccPoint& obj) const -> bool;
  auto operator!=(const EccPoint& obj) const -> bool;
  auto operator-() const -> EccPoint;
  auto isInfinity() const -> bool;
  template<class T>
  friend auto operator<<(std::ostream& os, const EccPoint<T>& obj)
    -> std::ostream&;
  friend Ecc<F>;
};

/**
 * @brief 楕円曲線暗号系クラス
 * @detail 楕円曲線 y^2 = x^3 + ax + b
 *         有限体Fをテンプレート引数とする
 *         楕円曲線暗号系での加算と乗算を行う
 */
template<class F>
class Ecc {
 private:
  F a, b;
 public:
  Ecc() {}
  Ecc(const F& a, const F& b) : a(a), b(b) {}
  auto add(const EccPoint<F>& obj1, const EccPoint<F>& obj2) const
    -> EccPoint<F>;
  template<class U>
  auto mult(const EccPoint<F>& obj, U n) const -> EccPoint<F>;
  auto calcLeft(const F& y) const -> F;
  auto calcRight(const F& x) const -> F;
  auto isIncluded(const EccPoint<F>& obj) const -> bool;
  static auto getParam(F& x, F& y, F& a, F& b) -> void;
};

/**
 * @brief 楕円曲線暗号ユーザクラス
 * @detail 楕円曲線暗号で利用する鍵を管理
 *         有限体Fをテンプレート引数とする
 *         初期化時に秘密鍵のみ生成
 */
template<class F>
class EccUser {
 private:
  EccPoint<F> common_key;
  decltype(F::order()) secret_key;
 public:
  Ecc<F> ecc;
  EccPoint<F> public_key, partner_key, base;
  EccUser();
  auto setSecretKey() -> void;
  auto setPublicKey() -> void;
  auto setCommonKey() -> void;
  auto send(EccUser& target) -> void;
  auto print(std::ostream& os) const -> void;
};

/**
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有クラス
 * @detail EccUserクラスのアリス, ボブ間で鍵を共有
 */
template<class F>
class Ecdh {
 private:
  auto setEcc() -> void;
 public:
  EccUser<F> alice, bob;
  Ecdh();
  auto publish() -> void;
  auto calc() -> void;
  auto print(std::ostream& os = std::cout) const -> void;
};

template<class F>
FInf<F>::FInf() {
  this->val = std::numeric_limits<decltype(F::order())>::max();
}

template<class F>
auto EccPoint<F>::operator==(const EccPoint& obj) const -> bool {
  return this->x == obj.x & this->y == obj.y;
}

template<class F>
auto EccPoint<F>::operator!=(const EccPoint& obj) const -> bool {
  return !(*this == obj);
}

template<class F>
auto EccPoint<F>::operator-() const -> EccPoint<F> {
  return EccPoint(this->x, -this->y);
}

template<class F>
inline auto EccPoint<F>::isInfinity() const -> bool {
  return this->x == FInf<F>() & this->y == FInf<F>();
}

template<class F>
auto operator<<(std::ostream& os, const EccPoint<F>& obj) -> std::ostream& {
  if(obj.isInfinity())
    os << "(inf,inf)";
  else
    os << '(' << obj.x << ',' << obj.y << ')';
  return os;
}

template<class F>
auto Ecc<F>::add(const EccPoint<F>& obj1, const EccPoint<F>& obj2) const
  -> EccPoint<F> {
  assert(this->isIncluded(obj1) && this->isIncluded(obj2));
  if(obj1.isInfinity()) return obj2;
  if(obj2.isInfinity()) return obj1;
  if(obj1 == -obj2) return EccPoint<F>();
  F lambda = obj1 == obj2?
    (F(3) * ::pow(obj1.x, 2) + this->a) / (F(2) * obj1.y):
    (obj2.y - obj1.y) / (obj2.x - obj1.x);
  F x_r = ::pow(lambda, 2) - obj1.x - obj2.x;
  F y_r = lambda * (obj1.x - x_r) - obj1.y;
  return EccPoint<F>(x_r, y_r);
}

template<class F> template<class U>
auto Ecc<F>::mult(const EccPoint<F>& obj, U n) const -> EccPoint<F> {
  assert(this->isIncluded(obj));
  if(!n || obj.isInfinity()) return EccPoint<F>();
  EccPoint<F> res = this->mult(this->add(obj, obj), n >> 1);
  if(n & 1) res = this->add(res, obj);
  return res;
}

template<class F>
inline auto Ecc<F>::calcLeft(const F& y) const -> F {
  return ::pow(y, 2);
}

template<class F>
inline auto Ecc<F>::calcRight(const F& x) const -> F {
  return ::pow(x, 3) + this->a * x + this->b;
}

template<class F>
inline auto Ecc<F>::isIncluded(const EccPoint<F>& obj) const -> bool {
  return obj.isInfinity() | this->calcLeft(obj.y) == this->calcRight(obj.x);
}

template<class F>
auto Ecc<F>::getParam(F& x, F& y, F& a, F& b) -> void {
  std::random_device rnd;
  std::mt19937_64 mt(rnd());
  std::uniform_int_distribution<decltype(F::order())>
    rnd_val(0, F::order() - 1);
  auto f = [&]{return F(rnd_val(mt));};
  do {
    x = f(), y = f(), a = f();
    b = F(::pow(y, 2) - (::pow(x, 3) + a * x));
  } while(F(4) * ::pow(a, 3) + F(27) * ::pow(b, 2) == F(0));
}

template<class F>
EccUser<F>::EccUser() {
  this->setSecretKey();
}

template<class F>
auto EccUser<F>::setSecretKey() -> void {
  std::random_device rnd;
  std::mt19937_64 mt(rnd());
  std::uniform_int_distribution<decltype(F::order())>
    rnd_val(1, F::order() - 1);
  this->secret_key = rnd_val(mt);
}

template<class F>
inline auto EccUser<F>::setPublicKey() -> void {
  this->public_key = this->ecc.mult(this->base, this->secret_key);
}

template<class F>
inline auto EccUser<F>::setCommonKey() -> void {
  this->common_key = this->ecc.mult(this->partner_key, this->secret_key);
}

template<class F>
inline auto EccUser<F>::send(EccUser<F>& target) -> void {
  target.partner_key = this->public_key;
}

template<class F>
auto EccUser<F>::print(std::ostream& os) const -> void {
  os << std::setw(10) << this->secret_key << "\t" << this->public_key
     << "\t" << this->common_key << std::endl;
}

template<class F>
Ecdh<F>::Ecdh() {
  this->setEcc();
  this->alice.setPublicKey();
  this->bob.setPublicKey();
}

template<class F>
auto Ecdh<F>::setEcc() -> void {
  F x, y, a, b;
  std::random_device rnd;
  std::mt19937_64 mt(rnd());
  std::uniform_int_distribution<decltype(F::order())>
    rnd_val(0, F::order() - 1);
  auto f = [&]{return F(rnd_val(mt));};
  do {
    x = f(), y = f(), a = f();
    b = F(::pow(y, 2) - (::pow(x, 3) + a * x));
  } while(F(4) * ::pow(a, 3) + F(27) * ::pow(b, 2) == F(0));
  Ecc<F> ecc(a, b);
  EccPoint<F> base(x, y);
  this->alice.ecc = ecc, this->bob.ecc = ecc;
  this->alice.base = base, this->bob.base = base;
}

template<class F>
inline auto Ecdh<F>::publish() -> void {
  this->alice.send(this->bob);
  this->bob.send(this->alice);
}

template<class F>
inline auto Ecdh<F>::calc() -> void {
  this->alice.setCommonKey();
  this->bob.setCommonKey();
}

template<class F>
auto Ecdh<F>::print(std::ostream& os) const -> void {
  os << "* Status" << std::endl;
  os << "\t" << "Secret key\t" << "Public key\t\t" << "Common key" << std::endl;
  os << "Alice\t";
  this->alice.print(os);
  os << "Bob\t";
  this->bob.print(os);
}
