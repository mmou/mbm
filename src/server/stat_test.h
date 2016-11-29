#ifndef SERVER_STAT_TEST_H
#define SERVER_STAT_TEST_H

#include <stdint.h>

#include "utils/result.h"

namespace mbm {
  class StatTest {
    public:
      StatTest(uint64_t target_run_length);
      StatTest(uint64_t target_run_length, double alpha, double beta);
      Result test_result(uint32_t n, uint32_t loss);

    private:
      void init(uint64_t target_run_length, double alpha, double beta);
      double h1;
      double h2;
      double s;

  };
} // namespace mbm

#endif // SERVER_STAT_TEST_H
