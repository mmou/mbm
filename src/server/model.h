#ifndef MODEL_H
#define MODEL_H

#include "utils/constants.h"

#include <stdint.h>
#include <algorithm>

namespace mbm {
namespace model {

	uint64_t target_window_size(uint32_t rate_kbs, uint32_t rtt_ms, uint32_t mss_bytes);

	uint64_t target_run_length(uint32_t rate_kbs, uint32_t rtt_ms, uint32_t mss_bytes);
    
} // namespace model
} // namespace mbm

#endif // MODEL_H 