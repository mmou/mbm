#ifndef CBR_H
#define CBR_H

#include <stdint.h>

#include "utils/config.h"
#include "utils/result.h"


#define MAX_TCPOPT          256

namespace mbm {
class Socket;

Result RunCBR(const Socket* client_mbm_socket,
              const Socket* client_control_socket,
              const Config& mbm_config);
}  // namespace mbm

#endif // CBR_H
