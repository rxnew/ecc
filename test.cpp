#include "ecc.hpp"

auto main() -> int {
  using LL = long long;
  //constexpr LL q = PrimeGenerator::create(84633113LL);
  constexpr LL q = PrimeGenerator::create(846331LL);
  using F = F<LL, q>;
  EccTest<F>::execute();
  return 0;
}
