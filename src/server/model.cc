#include "server/model.h"

#include <stdint.h>
#include <algorithm>
#include "utils/constants.h"

namespace mbm {
namespace model {


uint64_t target_pipe_size(int rate_kbs, int rtt_ms, int mss_bytes) {
	// TARGET_WINDOW_SIZE (target_pipe_size) = rate * RTT / (MTU - header_overhead)

	// convert kilobits per second to bytes per milisecond
	uint64_t rate_bytes_ms = rate_kbs / 8;
	return std::max(rate_bytes_ms * rtt_ms / mss_bytes, static_cast<uint64_t>(MIN_TARGET_WINDOW_SIZE));
}

uint64_t target_run_length(int rate_kbs, int rtt_ms, int mss_bytes) {
	// TARGET_RUN_LENGTH (target_run_length) = 3 *(TARGET_WINDOW_SIZE)^2
	// estimate of min required # of unmarked packets that must be delivered btwn losses/ECN marks

	uint64_t window_size = target_window_size(rate_kbs, rtt_ms, mss_bytes);
	return 3 * window_size * window_size;
}

} // namesapce model
} // namespace mbm
