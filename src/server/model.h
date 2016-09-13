#ifndef MODEL_H
#define MODEL_H

#include "utils/constants.h"

#include <stdint.h>
#include <algorithm>

namespace mbm {
namespace model {

	uint64_t target_window_size(int rate_kbs, int rtt_ms, int mss_bytes);

	uint64_t target_run_length(int rate_kbs, int rtt_ms, int mss_bytes);
    
} // namespace model
} // namespace mbm

#endif // MODEL_H 