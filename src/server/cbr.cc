#include "cbr.h"
#include "common/config.h"

namespace mbm {

Result RunCBR(int client_mbm_socket,
              int client_control_socket,
              const mbm_config& mbm_config) {

  // get server and client ip address

//
//  std::cout << "server ip addr: " << server_str << std::endl;
//  std::cout << "client ip addr: " << client_str << std::endl;
//  
//  switch (test_socket->type()) {
//    case SOCKETTYPE_TCP:
//      if (getsockopt(test_socket->raw(), IPPROTO_TCP, TCP_MAXSEG,
//                     &tcp_mss, &mss_len) != 0) {
//        if (errno != ENOPROTOOPT) {
//          std::cerr << "Failed to get TCP_MAXSEG: " << strerror(errno) << "\n";
//          return RESULT_ERROR;
//        }
//        std::cout << "Socket does not support TCP_MAXSEG opt. "
//                  << "Using default " << TCP_MSS << std::endl;
//      }
//      break;
//
//    case SOCKETTYPE_UDP:
//      tcp_mss = TCP_MSS;
//      break;
//
//    default:
//      std::cerr << "Unknown socket_type: " << test_socket->type() << "\n";
//      return RESULT_ERROR;
//  }
//
//  if (config.cbr_kb_s == 0) {
//    std::cerr << "Rate should be > 0\n";
//    return RESULT_ERROR;
//  }
//
//  // we get rate as kilobits per second, turn that into bytes per second
//  uint32_t bytes_per_sec = ((config.cbr_kb_s * 1000) / 8);
//
//  // we're going to meter the bytes into the socket interface in units of
//  // tcp_mss
//  uint32_t bytes_per_chunk = config.mss_bytes;
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    bytes_per_chunk = std::min(config.mss_bytes, tcp_mss);
//  }
//
//  // calculate how many chunks per second we want to send
//  uint32_t chunks_per_sec = bytes_per_sec / bytes_per_chunk;
//
//  // calculate how many ns per chunk
//  uint64_t time_per_chunk_ns = NS_PER_SEC / chunks_per_sec;
//
//  // calculate how many sec per chunk
//  double time_per_chunk_sec = 1.0 / chunks_per_sec;
//
//  // calculate the burst size for sleep time to be greater than 500us
//  // if the burst size from config is greater, use the config burst size
//  uint32_t burst_size_pkt = std::max(1000000 / time_per_chunk_ns,
//                                     static_cast<uint64_t>(config.burst_size));
//
//  // calculate the maximum test time
//  uint32_t max_test_time_sec =
//    std::min(TEST_BASE_SEC + TEST_INCR_SEC_PER_MB * config.cbr_kb_s / 1000,
//             static_cast<unsigned>(TEST_MAX_SEC));
//  uint32_t max_cwnd_time_sec =
//    std::min(CWND_BASE_SEC + CWND_INCR_SEC_PER_MB * config.cbr_kb_s / 1000,
//             static_cast<unsigned>(CWND_MAX_SEC));
//  if (test_socket->type() == SOCKETTYPE_UDP)
//    max_cwnd_time_sec = 0;
//  uint32_t max_test_pkt = max_test_time_sec * chunks_per_sec;
//  uint32_t max_cwnd_pkt = max_cwnd_time_sec * chunks_per_sec;
//
//  // calculate the target parameters
//  uint64_t target_pipe_size = model::target_pipe_size(config.cbr_kb_s,
//                                                      config.rtt_ms,
//                                                      config.mss_bytes);
//  uint64_t target_run_length = model::target_run_length(config.cbr_kb_s,
//                                                        config.rtt_ms,
//                                                        config.mss_bytes);
//  uint64_t target_pipe_size_bytes = target_pipe_size * config.mss_bytes;
//  uint64_t rtt_ns = config.rtt_ms * 1000 * 1000;
//
//  // Sending chunk length and traffic volume to client
//  ssize_t num_bytes;
//  if (!ctrl_socket->Send(mlab::Packet(htonl(bytes_per_chunk)), &num_bytes))
//    return RESULT_ERROR;
//  if (!ctrl_socket->Send(mlab::Packet(
//        htonl(max_test_pkt + max_cwnd_pkt)), &num_bytes))
//    return RESULT_ERROR;
//  if (!ctrl_socket->Send(mlab::Packet(
//        htonl(max_test_time_sec + max_cwnd_time_sec)), &num_bytes))
//    return RESULT_ERROR;
//
//  // traffic pattern log
//  std::cout << "  tcp_mss: " << tcp_mss << "\n";
//  std::cout << "  bytes_per_sec: " << bytes_per_sec << "\n";
//  std::cout << "  bytes_per_chunk: " << bytes_per_chunk << "\n";
//  std::cout << "  chunks_per_sec: " << chunks_per_sec << "\n";
//  std::cout << "  time_per_chunk_ns: " << time_per_chunk_ns << "\n";
//  std::cout << "  time_per_chunk_sec: " << time_per_chunk_sec << "\n";
//  std::cout << "  burst_size_pkt: " << burst_size_pkt << "\n";
//  std::cout << "  target_pipe_size_pkt: " << target_pipe_size << "\n";
//  std::cout << "  target_run_length_pkt: " << target_run_length << "\n";
//
//  uint32_t cwnd_bytes_total = 0;
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    cwnd_bytes_total = bytes_per_chunk * max_cwnd_pkt;
//    std::cout << "  sending at most " << max_cwnd_pkt
//              << " packets (" << cwnd_bytes_total << " bytes)"
//              << " to grow cwnd" << std::endl;
//  }
//  uint32_t test_bytes_total = bytes_per_chunk * max_test_pkt;
//  std::cout << "  sending at most " << max_test_pkt
//            << " test packets (" << test_bytes_total << " bytes)\n";
//
//  // Maximum time for the traffic
//  std::cout << "  cwnd traffic should take at most "
//            << max_cwnd_time_sec << " seconds\n";
//  std::cout << "  test traffic should take at most "
//            << max_test_time_sec << " seconds\n";
//
//  // Initialize the web100 agent
//  #ifdef USE_WEB100
//  web100::Agent agent;
//  #endif // USE_WEB100
//
//  #ifdef USE_WEB100
//  TrafficGenerator growth_generator(test_socket, bytes_per_chunk, max_cwnd_pkt);
//  uint64_t growth_start_time = GetTimeNS();
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    web100::Connection growth_connection(test_socket, agent.get());
//    growth_connection.Start();
//    while (growth_generator.packets_sent() < max_cwnd_pkt) {
//      growth_connection.Stop();
//      if (growth_connection.CurCwnd() >= target_pipe_size_bytes) {
//        std::cout << "cwnd reached" << std::endl;
//        break;
//      }
//      if (!growth_generator.Send(target_pipe_size)) {
//        return RESULT_ERROR;
//      }
//      if (GetTimeNS() > growth_start_time
//          + static_cast<uint64_t>(max_cwnd_time_sec) * NS_PER_SEC) {
//        std::cout << "max time reached" << std::endl;
//        break;
//      }
//      NanoSleepX( rtt_ns / NS_PER_SEC, rtt_ns % NS_PER_SEC);
//    }
//    std::cout << "loss during growth: "
//              << growth_connection.PacketRetransCount() << std::endl;
//    std::cout << "growing phase done" << std::endl;
//
//    // uint32_t growth_rtt = growth_connection.SampleRTT();
//    while (growth_connection.SndNxt() - growth_connection.SndUna()
//            >= std::max(target_pipe_size_bytes / 2, static_cast<uint64_t>(1))) {
//      growth_connection.Stop();
//      // NanoSleepX(growth_rtt / MS_PER_SEC, (growth_rtt % MS_PER_SEC) * 1000000);
//    }
//    std::cout << "done draining" << std::endl;
//  }
//  #endif
//
//  // Start the test
//  StatTest tester(target_run_length);
//  TrafficGenerator generator(test_socket, bytes_per_chunk, max_test_pkt);
//
//  #ifdef USE_WEB100
//  web100::Connection test_connection(test_socket, agent.get());
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    test_connection.Start();
//  }
//  #endif
//
//  Result test_result = RESULT_INCONCLUSIVE;
//  bool result_set = false;
//  uint64_t outer_start_time = GetTimeNS();
//  uint64_t missed_total = 0;
//  uint64_t missed_max = 0;
//  uint32_t missed_sleep = 0;
//
//  while (generator.packets_sent() < max_test_pkt) {
//    if (!generator.Send(burst_size_pkt)) {
//      return RESULT_ERROR;
//    }
//
//    #ifdef USE_WEB100
//    if (test_socket->type() == SOCKETTYPE_TCP) {
//      // sample the data once a second
//      if (generator.packets_sent() % chunks_per_sec == 0) {
//        // statistical test
//        test_connection.Stop();
//        uint32_t loss = test_connection.PacketRetransCount();
//        uint32_t n = generator.packets_sent();
//        test_result = tester.test_result(n, loss);
//        if (test_result == RESULT_PASS) {
//          std::cout << "passed SPRT" << std::endl;
//          result_set = true;
//          break;
//        } else if (test_result == RESULT_FAIL) {
//          std::cout << "failed SPRT" << std::endl;
//          result_set = true;
//          break;
//        }
//      }
//    }
//    #endif
//    
//    // figure out the start time for the next chunk
//    uint64_t next_start = outer_start_time +
//                          generator.packets_sent() * time_per_chunk_ns;
//    uint64_t curr_time = GetTimeNS();
//    int32_t left_over_ns = next_start - curr_time;
//    if (left_over_ns > 0) {
//      // If we have time left over, sleep the remainder.
//      NanoSleepX(left_over_ns / NS_PER_SEC, left_over_ns % NS_PER_SEC);
//    } else {
//      missed_total += abs(left_over_ns);
//      missed_sleep++;
//      missed_max = std::max(missed_max, static_cast<uint64_t>(abs(left_over_ns)));
//      if (missed_total > (curr_time - outer_start_time) / 2) {
//        // Inconclusive because the test failed to generate the traffic pattern
//        test_result = RESULT_INCONCLUSIVE;
//        result_set = true;
//        break;
//      }
//    }
//  }
//
//  uint64_t outer_end_time = GetTimeNS();
//  uint64_t delta_time = outer_end_time - outer_start_time;
//  double delta_time_sec = static_cast<double>(delta_time) / NS_PER_SEC;
//
//  // wait for a rtt, so that end doesn't arrive too early
//  NanoSleepX( config.rtt_ms * 1000 * 1000 / NS_PER_SEC,
//              config.rtt_ms * 1000 * 1000 % NS_PER_SEC);
//  // notify the client that the test has ended
//  if (!ctrl_socket->Send(mlab::Packet(END), &num_bytes))
//    return RESULT_ERROR;
//
//  uint32_t lost_packets = 0;
//  #ifdef USE_WEB100
//  // Traffic statistics from web100
//  uint32_t application_write_queue = 0;
//  uint32_t retransmit_queue = 0;
//  uint32_t rtt_ms = 0;
//  double rtt_sec = 0.0;
//
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    test_connection.Stop();
//    lost_packets = test_connection.PacketRetransCount();
//    application_write_queue = test_connection.ApplicationWriteQueueSize();
//    retransmit_queue = test_connection.RetransmitQueueSize();
//    rtt_ms = test_connection.SampleRTT();
//    rtt_sec = static_cast<double>(rtt_ms) / MS_PER_SEC;
//  }
//  #endif
//
//  // Observed data rates
//  double send_rate = (generator.total_bytes_sent() * 8) / delta_time_sec;
//  double send_rate_delta_percent = (send_rate * 100) / (bytes_per_sec * 8);
//
//  // Sleep statistics
//  std::cout << "Sleep missed" << std::endl;
//  std::cout << "maximum: " << missed_max << std::endl;
//  std::cout << "average: "
//            << (missed_sleep == 0? 0 : missed_total / missed_sleep)
//            << std::endl;
//  std::cout << "count: " << missed_sleep << std::endl;
//
//  // Receive the data collected by the client
//  uint32_t data_size_obj;
//  mlab::Packet data_size_pkt =
//    ctrl_socket->Receive(sizeof(data_size_obj), &num_bytes);
//  if (num_bytes < 0 || static_cast<unsigned>(num_bytes) < sizeof(data_size_obj))
//    return RESULT_ERROR;
//  data_size_obj = ntohl(data_size_pkt.as<uint32_t>());
//
//  uint32_t data_size_bytes = data_size_obj * sizeof(TrafficData);
//  std::cout << "client data: " << data_size_bytes << " bytes" << std::endl;
//
//  std::vector<TrafficData> client_data(data_size_obj);
//  std::vector<uint8_t> bytes_buffer;
//  uint32_t total_recv_bytes = 0;
//  while (total_recv_bytes < data_size_bytes) {
//    uint32_t num_to_receive = std::min(static_cast<unsigned>(MAX_RECV_BYTES),
//                                       data_size_bytes - total_recv_bytes);
//    mlab::Packet recv_pkt = ctrl_socket->Receive(num_to_receive, &num_bytes);
//    if (num_bytes <= 0)
//      return RESULT_ERROR;
//    bytes_buffer.insert(bytes_buffer.end(),
//                        recv_pkt.buffer(),
//                        recv_pkt.buffer() + num_bytes);
//    total_recv_bytes += num_bytes;
//  }
//  std::cout << "data collected" << std::endl;
//  const TrafficData* recv_buffer =
//      reinterpret_cast<const TrafficData*>(&bytes_buffer[0]);
//  for (uint32_t i=0; i < data_size_obj; ++i) {
//    client_data[i] = TrafficData::ntoh(recv_buffer[i]);
//  }
//
//
//  if (test_socket->type() == SOCKETTYPE_UDP) {
//    lost_packets = generator.packets_sent() - data_size_obj;
//  }
//
//  std::cout << "\npackets sent: " << generator.packets_sent() << "\n";
//  std::cout << "bytes sent: " << generator.total_bytes_sent() << "\n";
//  std::cout << "time: " << delta_time_sec << "\n";
//  std::cout << "send rate: " << send_rate << " b/sec ("
//            << send_rate_delta_percent << "% of target)\n";
//
//  #ifdef USE_WEB100
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    std::cout << "  lost: " << lost_packets << "\n";
//    std::cout << "  write queue: " << application_write_queue << "\n";
//    std::cout << "  retransmit queue: " << retransmit_queue << "\n";
//    std::cout << "  rtt: " << rtt_sec << "\n";
//
//    if (rtt_sec > 0.0) {
//      if ((application_write_queue + retransmit_queue) / rtt_sec <
//          bytes_per_sec) {
//        std::cout << "  kept up\n";
//      } else {
//        std::cout << "  failed to keep up\n";
//      }
//    }
//  }
//  #endif
//  if (test_socket->type() == SOCKETTYPE_UDP) {
//    std::cout << "  lost: " << lost_packets << "\n";
//  }
//
//  // determine the result of the test
//  if (test_socket->type() == SOCKETTYPE_UDP && !result_set)
//    test_result = tester.test_result(generator.packets_sent(), lost_packets);
//
//  // print the result, and send it to the client
//  if (test_result == RESULT_ERROR)
//    std::cerr << kResultStr[test_result] << "\n";
//  else
//    std::cout << kResultStr[test_result] << "\n";
//
//  if (!ctrl_socket->Send(mlab::Packet(htonl(test_result)), &num_bytes)) {
//    std::cout << "failed to send result" << std::endl;
//    return RESULT_ERROR;
//  }
//
//
//  
//  // setup log file directory
//  struct timespec test_time;
//  clock_gettime(CLOCK_REALTIME, &test_time);
//  struct tm* time_tm = gmtime(&test_time.tv_sec);
//  char buffer[20];
//  strftime(buffer, sizeof(buffer), "/%Y/%m/%d/", time_tm);
//  std::stringstream ss;
//  ss << FLAGS_prefix << buffer;
//
//  boost::system::error_code ec;
//  boost::filesystem::create_directories(ss.str(), ec);
//
//  // generate log file name
//  strftime(buffer, sizeof(buffer), "%Y%m%dT%T.", time_tm);
//  ss << buffer << test_time.tv_nsec << 'Z';
//  std::string file_name_prefix = ss.str();
//
//  // log the test configuration and summary data
//  std::ofstream fs_test;
//  fs_test.open((file_name_prefix + "_testdata").c_str());
//  fs_test << "server_ip_addr " << server_str << std::endl;
//  fs_test << "client_ip_addr " << client_str << std::endl;
//  fs_test << "socket_type "
//          << (test_socket->type() == SOCKETTYPE_TCP? "tcp": "udp") << std::endl;
//  fs_test << "target_rate_kb_s " << config.cbr_kb_s << std::endl;
//  fs_test << "target_rtt_ms " << config.rtt_ms << std::endl;
//  fs_test << "target_mss_bytes " << config.mss_bytes << std::endl;
//  fs_test << "target_pipe_size_pkt " << target_pipe_size << std::endl;
//  fs_test << "target_runlength_pkt " << target_run_length << std::endl;
//  fs_test << "packet_size " << bytes_per_chunk << std::endl;
//  fs_test << "ns_per_packet " << time_per_chunk_ns << std::endl;
//  fs_test << "packets_sent " << generator.packets_sent() << std::endl;
//  fs_test << "bytes_sent " << generator.total_bytes_sent() << std::endl;
//  fs_test << "total_time_ns " << delta_time << std::endl;
//  fs_test << "send_rate_bits_sec " << send_rate << std::endl;
//  fs_test << "missed_sleep_count " << missed_sleep << std::endl;
//  fs_test << "missed_sleep_maximum_ns " << missed_max << std::endl;
//  fs_test << "missed_sleep_average_ns "
//          << (missed_sleep == 0? 0 : missed_total / missed_sleep) << std::endl;
//  fs_test << "packet_loss " << lost_packets << std::endl;
//  fs_test << "type_I_err " << DEFAULT_TYPE_I_ERR << std::endl;
//  fs_test << "type_II_err " << DEFAULT_TYPE_II_ERR << std::endl;
//  fs_test << "test_result " << kResultStr[test_result] << std::endl;
//  #if USE_WEB100
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    fs_test << "write_queue_at_end " << application_write_queue << std::endl;
//    fs_test << "retransmit_queue_at_end " << retransmit_queue << std::endl;
//    fs_test << "sample_rtt_ms " << rtt_ms << std::endl;
//  }
//  #endif
//  fs_test.close();
//  // log the client data
//  std::ofstream fs_client;
//  fs_client.open((file_name_prefix + "_clientdata").c_str());
//  // seq_no, nonce and timestamp
//  for (std::vector<TrafficData>::const_iterator it = client_data.begin();
//       it != client_data.end(); ++it) {
//    fs_client << it->seq_no() << ' ' << it->nonce()
//              << ' ' << it->timestamp() << std::endl;
//  }
//  fs_client.close();
//  // log the server data
//  std::ofstream fs_server;
//  fs_server.open((file_name_prefix + "_serverdata").c_str());
//  // seq_no, nonce and timestamp
//  #ifdef USE_WEB100
//  if (test_socket->type() == SOCKETTYPE_TCP) {
//    for (uint32_t i=0; i < growth_generator.packets_sent(); ++i) {
//      fs_server << i << ' ' << growth_generator.nonce()[i]
//                << ' ' << growth_generator.timestamps()[i] << std::endl;
//    }
//  }
//  #endif
//  for (uint32_t i=0; i < generator.packets_sent(); ++i) {
//    fs_server << i << ' ' << generator.nonce()[i]
//              << ' ' << generator.timestamps()[i] << std::endl;
//  }
//  fs_server.close();
//  
//  std::cout << "Done CBR" << std::endl;
//  return test_result;
}

}  // namespace mbm
