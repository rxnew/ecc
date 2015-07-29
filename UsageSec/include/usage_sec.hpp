#ifndef __USAGESEC_HPP__
#define __USAGESEC_HPP__

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include <sys/resource.h>

class UsageSec {
 private:
  double start_time_;
  double stop_time_;
  double getSec() const;
 public:
  UsageSec();
  ~UsageSec();
  void start();
  void stop();
  double getUsageSec() const;
  void print(std::ostream& os) const;
};

#endif
