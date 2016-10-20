#include "server/model.h"

#include <stdio.h>
#include <stdint.h>
#include <algorithm>

namespace mbm {
namespace model {


uint64_t target_window_size(uint32_t rate_kbs, uint32_t rtt_ms, uint32_t mss_bytes) {
	// TARGET_WINDOW_SIZE (target_window_size) = rate * RTT / (MTU - header_overhead)

	// convert kilobits per second to bytes per milisecond
	uint64_t rate_bytes_ms = rate_kbs / 8;
	uint64_t window_size = std::max(rate_bytes_ms * rtt_ms / mss_bytes, static_cast<uint64_t>(MIN_TARGET_WINDOW_SIZE));
	fprintf(stdout, "===> TARGET WINDOW SIZE: %u\n", window_size);
	return window_size;
}

uint64_t target_run_length(uint32_t rate_kbs, uint32_t rtt_ms, uint32_t mss_bytes) {
	// TARGET_RUN_LENGTH (target_run_length) = 3 *(TARGET_WINDOW_SIZE)^2
	// estimate of min required # of unmarked packets that must be delivered btwn losses/ECN marks

	uint64_t window_size = target_window_size(rate_kbs, rtt_ms, mss_bytes);
	uint64_t run_length = 3 * window_size * window_size;
	fprintf(stdout, "===> TARGET RUN LENGTH: %u\n", run_length);
	return run_length;
}

} // namesapce model
} // namespace mbm
