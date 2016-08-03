#include "common/config.h"

#include <assert.h>
#include <stdint.h>

#include <string>

namespace mbm {

	Config::Config() {
		rate = 0;
		rtt = 0;
		mss = 0;
		burst_size = 1;		
	}

	Config::Config(int rate, int rtt, int mss, int burst_size) {
		rate = rate;
		rtt = rtt;
		mss = mss;
		burst_size = burst_size;		
	}

}  // namespace mbm