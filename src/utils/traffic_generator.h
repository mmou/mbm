#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H


#include "socket.h"

#include <vector>


#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>

#include "socket.h"
#include "packet.h"
#include "constants.h"
#include "time.h"

namespace mbm {

class TrafficGenerator {  
  public:
    TrafficGenerator(const Socket *test_socket,
                     uint32_t bytes_per_chunk, uint32_t max_pkt);
    bool Send(uint32_t num_chunks, ssize_t& num_bytes);
    bool Send(uint32_t num_chunks);
    uint32_t packets_sent();
    uint64_t total_bytes_sent();
    uint32_t bytes_per_chunk();
    const std::vector<uint32_t>& nonce();
    const std::vector<uint64_t>& timestamps();

  private:
    const Socket *test_socket_;
    uint32_t max_packets_;
    uint32_t bytes_per_chunk_;
    uint64_t total_bytes_sent_;
    uint32_t packets_sent_;
    uint32_t last_percent_;
    std::vector<char> buffer_;
    std::vector<uint32_t> nonce_;
    std::vector<uint64_t> timestamps_;
    
};

} // namespace mbm

#endif  // TRAFFIC_GENERATOR_H