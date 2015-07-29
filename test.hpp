/**
 * @file test.hpp
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有テストクラスヘッダ
 * @date 2015.07.29
 */

#include <iostream>
#include "ecc.hpp"
#include "UsageSec/src/usage_sec.hpp"

/**
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有テストクラス
 * @detail Ecdhクラスのメソッドを使用して鍵交換を行う
 *         公開された情報からクラッキングを行う
 */
template<class F>
class EcdhTest {
 private:
  Ecdh<F> ecdh;
  auto exchange(std::ostream& os = std::cout) -> void;
  auto crack(std::ostream& os = std::cout) const -> void;
 public:
  EcdhTest() : ecdh() {}
  auto execute() -> void;
};

template<class F>
auto EcdhTest<F>::exchange(std::ostream& os) -> void {
  this->ecdh.publish();      // 公開鍵を公開
  this->ecdh.calc();         // 共通鍵を計算
  this->ecdh.print(os);
}

template<class F>
auto EcdhTest<F>::crack(std::ostream& os) const -> void {
  os << "Cracking now ..." << std::flush;
  decltype(F::order()) alice_secret_key(1);
  const Ecc<F>& ecc = this->ecdh.alice.ecc;
  const EccPoint<F>& base = this->ecdh.alice.base;
  EccPoint<F> tmp = base;
  while(tmp != this->ecdh.alice.public_key)
    tmp = ecc.add(tmp, base), alice_secret_key++;
  EccPoint<F> common_key =
    ecc.mult(this->ecdh.bob.public_key, alice_secret_key);
  os << " done." << std::endl;
  os << "+ Alice's secret key\t" << alice_secret_key << std::endl;
  os << "+ Common key\t\t" << common_key << std::endl;
}

template<class F>
auto EcdhTest<F>::execute() -> void {
  UsageSec timer;
  timer.start();
  this->exchange();          // 鍵交換
  timer.stop();
  timer.print(std::cout);
  std::cout << std::endl;
  timer.start();
  this->crack();             // 第三者による解読
  timer.stop();
  timer.print(std::cout);
}
