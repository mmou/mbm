/* from mbm */

#ifndef RESULT_H
#define RESULT_H

namespace mbm {
enum Result {
  RESULT_FAIL,
  RESULT_PASS,
  RESULT_INCONCLUSIVE,
  RESULT_ERROR,
  NUM_RESULTS
};

extern const char* kResultStr[NUM_RESULTS];
}  // namespace mbm

#endif  // RESULT_H
