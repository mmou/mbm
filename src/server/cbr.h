#include "utils/result.h"

namespace mbm {
class mbm_config;

Result RunCBR(Socket* client_mbm_socket,
              Socket* client_control_socket,
              const Config& mbm_config);
}  // namespace mbm