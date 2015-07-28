#include "ecc.hpp"
#include "UsageSec/src/usage_sec.hpp"

template<class F>
class EccTest;

template<class F>
auto EccTest<F>::execute() -> void {
  UsageSec gen_time, crack_time;
  gen_time.start();
  EccNetwork<F> comm;
  comm.publish();
  comm.calc();
  gen_time.stop();
  comm.print();
  gen_time.print(std::cout);
  std::cout << std::endl;
  crack_time.start();
  comm.crack();
  crack_time.stop();
  crack_time.print(std::cout);
}

auto main() -> int {
  using LL = long long;
  constexpr LL q = PrimeGenerator::create(84633113LL);
  //constexpr LL q = PrimeGenerator::create(8463312LL);
  using F = F<LL, q>;
  EccTest<F>::execute();
  return 0;
}
