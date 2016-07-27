#include "common/config.h"

#include <assert.h>
#include <stdint.h>

#include <string>

namespace mbm {

  // struct or class?
  struct mbm_config {
      int rate;
      int rtt;
      int mss;
      int burst_size;
  };

}  // namespace mbm
