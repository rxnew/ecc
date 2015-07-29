/**
 * @file test.cpp
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有テストの実装
 * @date 2015.07.29
 */

#include "ecc.hpp"
#include "UsageSec/src/usage_sec.hpp"

/**
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有テストクラス
 */
template<class F>
struct EcdhTest {
  static auto execute() -> void;
};

template<class F>
auto EcdhTest<F>::execute() -> void {
  UsageSec gen_time, crack_time;
  gen_time.start();
  Ecdh<F> ecdh;              // 暗号系の初期化 (楕円曲線, 秘密鍵)
  ecdh.publish();            // 公開鍵を公開
  ecdh.calc();               // 共通鍵を計算
  gen_time.stop();
  ecdh.print();
  gen_time.print(std::cout);
  std::cout << std::endl;
  crack_time.start();
  ecdh.crack();              // 第三者による解読
  crack_time.stop();
  crack_time.print(std::cout);
}

auto main() -> int {
  using LL = long long;
  // q以上の素数pを生成
  constexpr LL q = 84633113LL;
  constexpr LL p = PrimeGenerator::create(q);
  using F = F<LL, p>;
  EcdhTest<F>::execute();
  return 0;
}
