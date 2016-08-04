#include "utils/result.h"

namespace mbm {
class mbm_config;

Result RunCBR(int client_mbm_socket,
              int client_control_socket,
              const mbm_config& mbm_config);
}  // namespace mbm