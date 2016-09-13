#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <string>

namespace mbm {

	class Config {
	public:
		Config();
		Config(int rate, int rtt, int mss, int burst_size);

		int rate;
		int rtt;
		int mss;
		int burst_size;
	  };

}  // namespace mbm

#endif  // CONFIG_H
