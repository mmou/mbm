#include "cbr.h"

#include "model.h"
#include "stat_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <netinet/tcp.h>

#include "utils/config.h"
#include "utils/constants.h"
#include "utils/traffic_generator.h"

#include <csignal>


namespace mbm {

/* Decode TCP options to string description. Please note that using strcat()
 * is highly dangerous and make sure you don't do it and never overwrite
 * your buffers! We use it here because we concatenate strings under our
 * control.
 */
void decode_tcp_options(char *options_text, u_int8_t tcp_options) {
    if ( (tcp_options & TCPI_OPT_TIMESTAMPS) ) {
        strcat(options_text, "Timestamps ");
    }
    if ( (tcp_options & TCPI_OPT_SACK) ) {
        strcat(options_text, "SACK ");
    }
    if ( (tcp_options & TCPI_OPT_WSCALE) ) {
        strcat(options_text, "Window Scaling ");
    }
    if ( (tcp_options & TCPI_OPT_ECN) ) {
        strcat(options_text, "ECN ");
    }
    return;
}



Result RunCBR(const Socket* client_mbm_socket,
              const Socket* client_control_socket,
              const Config& mbm_config) {

	fprintf(stdout, "IN CBR TEST: %u", mbm_config.rate);

	uint32_t rate_Bps = mbm_config.rate * 1000 / 8; // kilobits per sec --> bytes per sec
	uint32_t rate_bps = mbm_config.rate * 1000; // bits per sec
    uint32_t bytes_per_chunk = mbm_config.mss;

	uint32_t chunks_per_sec = std::max(static_cast<int>(rate_Bps / bytes_per_chunk), 1);	// calculate how many chunks per second we want to send
    uint64_t time_per_chunk_ns = NS_PER_SEC / chunks_per_sec;	// calculate how many ns per chunk	
    double time_per_chunk_sec = 1.0 / chunks_per_sec;	// calculate how many sec per chunk
  
    // calculate the burst size for sleep time to be greater than 500us (because we assume RTT > 1 ms???)
    // 1000000 ns / time_per_chunk_ns = number of packets in 1 ms.
    // use the larger burst size
    uint32_t burst_size_pkt = std::max(1000000 / time_per_chunk_ns, static_cast<uint64_t>(mbm_config.burst_size));

	// calculate the maximum test time
	uint32_t max_test_time_sec = std::min(static_cast<int>(TEST_BASE_SEC + TEST_INCR_SEC_PER_MB * mbm_config.rate / 1000), static_cast<int>(TEST_MAX_SEC));
	uint32_t max_cwnd_time_sec = std::min(static_cast<int>(CWND_BASE_SEC + CWND_INCR_SEC_PER_MB * mbm_config.rate / 1000), static_cast<int>(CWND_MAX_SEC));
	uint32_t max_test_pkt = max_test_time_sec * chunks_per_sec;
	uint32_t max_cwnd_pkt = max_cwnd_time_sec * chunks_per_sec;

	// calculate the target parameters
	uint64_t target_window_size = model::target_window_size(mbm_config.rate, mbm_config.rtt, mbm_config.mss);
	uint64_t target_run_length = model::target_run_length(mbm_config.rate, mbm_config.rtt, mbm_config.mss);
	uint64_t target_window_size_bytes = target_window_size * mbm_config.mss;
	uint64_t rtt_ns = mbm_config.rtt * 1000;

	// traffic pattern log
	fprintf(stdout, "rate_Bps: %d\n", rate_Bps);
    fprintf(stdout, "rate_Mbps: %d\n", rate_Bps * 8 / 1000 / 1000);    
	fprintf(stdout, "bytes_per_chunk: %d\n", bytes_per_chunk);
	fprintf(stdout, "chunks_per_sec: %d\n", chunks_per_sec);
	fprintf(stdout, "time_per_chunk_ns: %u\n", time_per_chunk_ns);
	fprintf(stdout, "time_per_chunk_sec: %u\n", time_per_chunk_sec);
	fprintf(stdout, "burst_size_pkt: %d\n", burst_size_pkt);
	fprintf(stdout, "target_window_size: %u\n", target_window_size);
	fprintf(stdout, "target_run_length: %u\n", target_run_length);

 	uint32_t cwnd_bytes_total = bytes_per_chunk * max_cwnd_pkt;
 	fprintf(stdout, "sending at most %d packets (%d bytes) to grow cwnd\n", max_cwnd_pkt, cwnd_bytes_total);
	uint32_t test_bytes_total = bytes_per_chunk * max_test_pkt;
	fprintf(stdout, "sending at most %d test packets (%d bytes)\n", max_test_pkt, test_bytes_total);

	// Maximum time for the traffic
	fprintf(stdout, "cwnd traffic should take at most %d seconds\n", max_cwnd_time_sec);
	fprintf(stdout, "test traffic should take at most %d seconds\n", max_test_time_sec);


//    std::raise(SIGINT);


    //fprintf(stdout, "sending bytes_per_chunk: %d\n", bytes_per_chunk);
	// Sending chunk length and traffic volume to client
	Packet bytes_per_chunk_packet(htonl(bytes_per_chunk));
	client_control_socket->sendOrDie(bytes_per_chunk_packet);

    //fprintf(stdout, "sending max_num_pkt: %d\n", max_test_pkt + max_cwnd_pkt);
	Packet max_num_pkt_packet(htonl(max_test_pkt + max_cwnd_pkt));
	client_control_socket->sendOrDie(max_num_pkt_packet);

    //fprintf(stdout, "sending max_time: %d\n", max_test_time_sec + max_cwnd_time_sec);
	Packet max_time_packet(htonl(max_test_time_sec + max_cwnd_time_sec));
	client_control_socket->sendOrDie(max_time_packet);

	Packet target_window_size_packet(htonl(target_window_size));
	client_control_socket->sendOrDie(target_window_size_packet);


	TrafficGenerator generator(client_mbm_socket, bytes_per_chunk, max_test_pkt);
    StatTest tester = StatTest(target_run_length);

	uint32_t send_rate_bps = 0;
	uint32_t ave_rate_bps = 0;
	uint32_t num_lost = 0;
	uint32_t num_retrans = 0;
    uint32_t num_total_retrans = 0;

	struct tcp_info tcp_info;			
    int tcp_info_length = sizeof(tcp_info);
	unsigned short opt_debug = 0;

    bool test_stream_ready = false;
    Result test_result = RESULT_INCONCLUSIVE;

	while (generator.packets_sent() < max_test_pkt) {
	    if (!generator.Send(1)) { // burst_size_pkt
	        printf("fail to send");
            test_result = RESULT_ERROR;
            break;                  
	    }

        /* Fill tcp_info structure with data to get the TCP options and the client's
         * name.
         */
        if ( getsockopt( client_mbm_socket->fd(), IPPROTO_TCP, TCP_INFO, (void *)&tcp_info, (socklen_t *)&tcp_info_length ) == 0 ) {

            send_rate_bps = tcp_info.tcpi_snd_cwnd * tcp_info.tcpi_snd_mss * 8 / tcp_info.tcpi_rtt * 1000 * 1000;
            ave_rate_bps = send_rate_bps*0.2 + ave_rate_bps*0.8;

            num_lost = num_lost + tcp_info.tcpi_lost;
            num_retrans = num_retrans + tcp_info.tcpi_retrans;
            num_total_retrans = num_total_retrans + tcp_info.tcpi_total_retrans;

            if (generator.packets_sent() > 0 && generator.packets_sent() % target_run_length == 0) {
                // test every target_run_length packets
                
                fprintf(stdout, "\n~~~~TCP INFO~~~~\n");
                fprintf(stdout, "SND RATE bps: %u\n", send_rate_bps);
                fprintf(stdout, "AVE RATE bps: %u\n", ave_rate_bps);            
                fprintf(stdout, "SND RATE Mbps: %u\n", send_rate_bps / 1000 / 1000);
                fprintf(stdout, "AVE RATE Mbps: %u\n", ave_rate_bps / 1000 / 1000);

                fprintf(stdout, "tcpi_lost: %u\n", tcp_info.tcpi_lost);
                fprintf(stdout, "tcpi_retrans: %u\n", tcp_info.tcpi_retrans);
                fprintf(stdout, "tcpi_retransmits: %u\n", tcp_info.tcpi_retransmits);
                fprintf(stdout, "tcpi_total_retrans: %u\n", tcp_info.tcpi_total_retrans);

                fprintf(stdout, "num packets_sent: %d\n", generator.packets_sent());
                fprintf(stdout, "num_lost: %d\n", num_lost);
                fprintf(stdout, "num_retrans: %d\n", num_retrans);
                fprintf(stdout, "num_total_retrans: %d\n", num_total_retrans);                

                if (ave_rate_bps >= rate_bps*0.9 || ave_rate_bps <= rate_bps*1.1) {
                    // if average rate is close enough to the target rate 

                    // if test stream wasn't ready, then it now is ready
                    if (!test_stream_ready) test_stream_ready = true;

                    if (generator.packets_sent() > max_test_pkt/3) {
                        // test only after a good amount of data is collected??
                        test_result = tester.test_result(generator.packets_sent(), num_lost);
                        if (test_result == RESULT_PASS) {
                            fprintf(stdout, "TEST PASSED\n");
                            break;
                        } else if (test_result == RESULT_FAIL) {
                            fprintf(stdout, "TEST FAILED\n");
                            break;
                        } else {
                            fprintf(stdout, "TEST so far inclusive\n");                        
                        }
                    }

                } else if (test_stream_ready) {
                    // if average rate is not close enough to target rate and test stream is supposed to be ready
                    fprintf(stdout, "TEST ERROR - FAILED TO ACHIEVE TARGET TEST STREAM RATE\n");
                    test_result = RESULT_ERROR;
                    break;
                }
            }
		}
	}

    Packet control_end_packet(END, strlen(END));
    client_control_socket->sendOrDie(control_end_packet);

    if (!(ave_rate_bps >= rate_bps*0.9 || ave_rate_bps <= rate_bps*1.1)) {
        fprintf(stdout, "TEST ERROR - FAILED TO ACHIEVE TARGET TEST STREAM RATE\n");
        test_result = RESULT_ERROR;
    }

    // client_mbm_socket sends test result
    Packet control_test_result_packet(test_result);
    client_control_socket->sendOrDie(control_test_result_packet);

    Packet send_rate_packet(htonl(send_rate_bps / 1000 / 1000));
    client_control_socket->sendOrDie(send_rate_packet);

    Packet ave_rate_packet(htonl(ave_rate_bps / 1000 / 1000));
    client_control_socket->sendOrDie(ave_rate_packet);

    Packet packets_sent_packet(htonl(generator.packets_sent()));
    client_control_socket->sendOrDie(packets_sent_packet);

    Packet num_lost_packet(htonl(num_lost));
    client_control_socket->sendOrDie(num_lost_packet);

    Packet num_retrans_packet(htonl(num_retrans));
    client_control_socket->sendOrDie(num_retrans_packet);

    Packet num_total_retrans_packet(htonl(num_total_retrans));
    client_control_socket->sendOrDie(num_total_retrans_packet);

    fprintf(stdout, "\nserver says TEST RESULT: %s\n", kResultStr[test_result]);
    fprintf(stdout, "SND RATE bps: %u\n", send_rate_bps);
    fprintf(stdout, "AVE RATE bps: %u\n", ave_rate_bps);            
    fprintf(stdout, "SND RATE Mbps: %u\n", send_rate_bps / 1000 / 1000);
    fprintf(stdout, "AVE RATE Mbps: %u\n", ave_rate_bps / 1000 / 1000);
    fprintf(stdout, "num packets_sent: %d\n", generator.packets_sent());
    fprintf(stdout, "num_lost: %d\n", num_lost);
    fprintf(stdout, "num_retrans: %d\n", num_retrans);
    fprintf(stdout, "num_total_retrans: %d\n", num_total_retrans);                


/*
growing window size phase
	web100::Connection growth_connection through test_socket/mbm_socket
	TrafficGenerator growth_generator

start test
	StatTest tester
	TrafficGenerator generator

	web100::Connection test_connection
	while packets sent < max_test_pkt:
		generator.Send
		tester.test_result
		sleep the calculated amount
	ctrl_socket: send END

test_connection traffic stats
	lost_packets
	application_write_queue
	retransmit_queue
	rtt_ms
	rtt_sec

observed data rates
	send_rate

sleep statistics

ctrl_socket: receive data_size_obj collected by client

tester.testresult

log
	fs_test: test configuration and summary data
	fs_client: client data
	fs_server: server data





*/

/*

TrafficGenerator growth_generator(test_socket, bytes_per_chunk, max_cwnd_pkt);
uint64_t growth_start_time = GetTimeNS();
if (test_socket->type() == SOCKETTYPE_TCP) {
  web100::Connection growth_connection(test_socket, agent.get());
  growth_connection.Start();
  while (growth_generator.packets_sent() < max_cwnd_pkt) {
    growth_connection.Stop();
    if (growth_connection.CurCwnd() >= target_pipe_size_bytes) {
      std::cout << "cwnd reached" << std::endl;
      break;
    }
    if (!growth_generator.Send(target_pipe_size)) {
      return RESULT_ERROR;
    }
    if (GetTimeNS() > growth_start_time
        + static_cast<uint64_t>(max_cwnd_time_sec) * NS_PER_SEC) {
      std::cout << "max time reached" << std::endl;
      break;
    }
    NanoSleepX( rtt_ns / NS_PER_SEC, rtt_ns % NS_PER_SEC);
  }

  std::cout << "loss during growth: "
            << growth_connection.PacketRetransCount() << std::endl;
  std::cout << "growing phase done" << std::endl;
  // uint32_t growth_rtt = growth_connection.SampleRTT();
  while (growth_connection.SndNxt() - growth_connection.SndUna()
          >= std::max(target_pipe_size_bytes / 2, static_cast<uint64_t>(1))) {
    growth_connection.Stop();
    // NanoSleepX(growth_rtt / MS_PER_SEC, (growth_rtt % MS_PER_SEC) * 1000000);
  }
  std::cout << "done draining" << std::endl;
}
*/


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
//  double send_rate_delta_percent = (send_rate * 100) / (rate_bps * 8);
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
//          rate_bps) {
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
