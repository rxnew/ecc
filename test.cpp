/**
 * @file test.cpp
 * @brief 楕円曲線ディフィー-ヘルマン鍵共有テストメインプログラム
 * @date 2015.07.29
 */

#include "test.hpp"

/**
 * @brief テストの実行
 * @detail Fp上の楕円曲線暗号系にてディフィー-ヘルマン鍵共有を行う
 *         pをq以上の最小の素数とする
 * @attention pはコンパイル時に決定する必要あり
 */
auto main() -> int {
  using LL = long long;
  constexpr LL q = 84633113LL;
  constexpr LL p = PrimeGenerator::create(q);
  using F = F<LL, p>;
  EcdhTest<F>().execute();
  return 0;
}
