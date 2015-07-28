#pragma once

#include <iostream>
#include <iomanip>
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
class EccNetwork;

/* 無限大 */
template<class F>
class FInf: public F {
 public:
  FInf();
};

/* 楕円曲線上の点 */
template<class F>
class EccPoint {
 private:
  F x, y;
 public:
  EccPoint(): x(FInf<F>()), y(FInf<F>()) {}
  template<class T>
  EccPoint(T x, T y): x(F(x)), y(F(y)) {}
  EccPoint(const F& x, const F& y): x(x), y(y) {}
  auto operator==(const EccPoint& obj) const -> bool;
  auto operator!=(const EccPoint& obj) const -> bool;
  auto operator-() const -> EccPoint;
  inline auto isInfinity() const -> bool {
    return this->x == FInf<F>() & this->y == FInf<F>();
  }
  template<class T>
  friend auto operator<<(std::ostream& os, const EccPoint<T>& obj)
    -> std::ostream&;
  friend Ecc<F>;
};

/* 楕円曲線 */
template<class F>
class Ecc {
 private:
  F a, b;
 public:
  Ecc() {}
  Ecc(const F& a, const F& b): a(a), b(b) {}
  auto add(const EccPoint<F>& obj1, const EccPoint<F>& obj2) const
    -> EccPoint<F>;
  template<class U>
  auto mult(const EccPoint<F>& obj, U n) const -> EccPoint<F>;
  inline auto calcLeft(const F& y) const -> F {
    return ::pow(y, 2);
  }
  inline auto calcRight(const F& x) const -> F {
    return ::pow(x, 3) + this->a * x + this->b;
  }
  inline auto isIncluded(const EccPoint<F>& obj) const -> bool {
    return obj.isInfinity() | this->calcLeft(obj.y) == this->calcRight(obj.x);
  }
  static auto getParam(F& x, F& y, F& a, F& b) -> void;
};

template<class F>
class EccUser {
 private:
  EccPoint<F> common_key;
  decltype(F::modulo()) secret_key;
 public:
  Ecc<F> ecc;
  EccPoint<F> public_key, partner_key, base;
  EccUser();
  auto setSecretKey() -> void;
  inline auto setPublicKey() -> void {
    public_key = ecc.mult(base, secret_key);
  }
  inline auto setCommonKey() -> void {
    common_key = ecc.mult(partner_key, secret_key);
  }
  inline auto send(EccUser& target) -> void {
    target.partner_key = public_key;
  }
  auto print(std::ostream& os) const -> void;
};

template<class F>
class EccNetwork {
 private:
  EccUser<F> alice, bob;
  auto setEcc() -> void;
 public:
  EccNetwork();
  inline auto publish() -> void {
    alice.send(bob), bob.send(alice);
  }
  inline auto calc() -> void {
    alice.setCommonKey(), bob.setCommonKey();
  }
  auto crack(std::ostream& os = std::cout) const -> void;
  auto print(std::ostream& os = std::cout) const -> void;
};

template<class F>
class EccTest {
 public:
  static auto execute() -> void;
};

template<class F>
FInf<F>::FInf() {
  this->val = std::numeric_limits<decltype(F::modulo())>::max();
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
auto Ecc<F>::getParam(F& x, F& y, F& a, F& b) -> void {
  std::random_device rnd;
  std::mt19937_64 mt(rnd());
  std::uniform_int_distribution<decltype(F::modulo())>
    rnd_val(0, F::modulo() - 1);
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
  std::uniform_int_distribution<decltype(F::modulo())>
    rnd_val(1, F::modulo() - 1);
  this->secret_key = rnd_val(mt);
}

template<class F>
auto EccUser<F>::print(std::ostream& os) const -> void {
  os << std::setw(10) << this->secret_key << "\t" << this->public_key
     << "\t" << this->common_key << std::endl;
}

template<class F>
EccNetwork<F>::EccNetwork() {
  this->setEcc();
  this->alice.setPublicKey();
  this->bob.setPublicKey();
}

template<class F>
auto EccNetwork<F>::setEcc() -> void {
  F x, y, a, b;
  std::random_device rnd;
  std::mt19937_64 mt(rnd());
  std::uniform_int_distribution<decltype(F::modulo())>
    rnd_val(0, F::modulo() - 1);
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
auto EccNetwork<F>::crack(std::ostream& os) const -> void {
  os << "Cracking now ..." << std::flush;
  decltype(F::modulo()) alice_secret_key(1);
  const Ecc<F>& ecc = this->alice.ecc;
  const EccPoint<F>& base = this->alice.base;
  EccPoint<F> tmp = base;
  while(tmp != this->alice.public_key)
    tmp = ecc.add(tmp, base), alice_secret_key++;
  EccPoint<F> common_key = ecc.mult(this->bob.public_key, alice_secret_key);
  os << " done." << std::endl;
  os << "+ Alice's secret key\t" << alice_secret_key << std::endl;
  os << "+ Common key\t\t" << common_key << std::endl;
}

template<class F>
auto EccNetwork<F>::print(std::ostream& os) const -> void {
  os << "* Status" << std::endl;
  os << "\t" << "Secret key\t" << "Public key\t\t" << "Common key" << std::endl;
  os << "Alice\t";
  this->alice.print(os);
  os << "Bob\t";
  this->bob.print(os);
}
