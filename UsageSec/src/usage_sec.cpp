#include "usage_sec.hpp"

using namespace std;

UsageSec::UsageSec() {
}

UsageSec::~UsageSec() {
}

double UsageSec::getSec() const {
  struct rusage t;
  struct timeval s;
  getrusage(RUSAGE_SELF, &t);
  s = t.ru_utime;
  return s.tv_sec + (double)s.tv_usec * 1e-6;
}

void UsageSec::start() {
  this->start_time_ = this->getSec();
}

void UsageSec::stop() {
  this->stop_time_ = this->getSec();
}

double UsageSec::getUsageSec() const {
  return this->stop_time_ - this->start_time_;
}

void UsageSec::print(ostream& os) const {
  os << "Usage time: " << fixed << setprecision(5) << this->getUsageSec() << endl;
}
