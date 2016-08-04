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