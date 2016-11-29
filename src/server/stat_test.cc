#include <server/stat_test.h>

#include <math.h>

#include <iostream>

#include "utils/constants.h"

namespace mbm {

StatTest::StatTest(uint64_t target_run_length) {
  init(target_run_length, DEFAULT_TYPE_I_ERR, DEFAULT_TYPE_II_ERR);
}

StatTest::StatTest(uint64_t target_run_length, double alpha, double beta) {
  init(target_run_length, alpha, beta);
}

void StatTest::init(uint64_t target_run_length, double alpha, double beta) {
  double p0 = 1.0 / target_run_length;  // H0:  p0 = one defect in target_run_length
  double p1 = std::min(1.0 / (target_run_length / 4.0), 0.99);  // H1:  p1 = one defect in target_run_length/4

  double k = log(p1 * (1 - p0) / (p0 * (1 - p1)));
  s = log((1-p0) / (1-p1)) / k;
  h1 = log((1-alpha) / beta) / k;
  h2 = log((1-beta) / alpha) / k;
}

Result StatTest::test_result(uint32_t n, uint32_t loss) {
  double xa = -h1 + s * n;  // acceptance line
  double xb = h2 + s * n;   // rejection line
  if (loss <= xa) return RESULT_PASS; // acept h2
  if (loss >= xb) return RESULT_FAIL; // accept h1
  return RESULT_INCONCLUSIVE; // continue testing
}

} // namespace mbm
