#include "utils/config.h"

#include <assert.h>
#include <stdint.h>

#include <string>

namespace mbm {

	Config::Config() {
		rate = 0; 	// specified app data rate ; kilobits per sec
		rtt = 0; 	// (round trip time) - specified min RTT of the longest complete path 
					// over which user expects target performance ; ms
		mss = 0;	// (max transmission unit) specified maximum MTU supported by complete 
					// path over which user expects target perf
					// 		Default = 1500 Byte MTU 
					// (mss = just the payload) ; bytes
		burst_size = 1;		
	}

	Config::Config(uint32_t rate, uint32_t rtt, uint32_t mss, uint32_t burst_size) {
		rate = rate;
		rtt = rtt;
		mss = mss;
		burst_size = burst_size;		
	}

}  // namespace mbm