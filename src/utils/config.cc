#include "utils/config.h"

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

	Config::Config(int rate, int rtt, int mss, int burst_size) {
		rate = rate;
		rtt = rtt;
		mss = mss;
		burst_size = burst_size;		
	}

}  // namespace mbm