#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <string>

namespace mbm {

	class Config {
	public:
		Config();
		Config(uint32_t rate, uint32_t rtt, uint32_t mss, uint32_t burst_size);

		uint32_t rate;
		uint32_t rtt;
		uint32_t mss;
		uint32_t burst_size;
	  };

}  // namespace mbm

#endif  // CONFIG_H
