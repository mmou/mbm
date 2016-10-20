#include "main.h"

#include "utils/config.h"
#include "utils/constants.h"
#include "utils/scoped_ptr.h"
#include "utils/socket.h"

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <getopt.h>

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

// const char default_filename[] = DEFAULT_FILENAME;

/* File descriptor for log file */
// FILE *statistics;

/* Things we need for dealing with TCP */
struct sockaddr_in client_control_address, server_listener_address;
char *tcp_buffer;
struct tcp_info tcp_info;
int tcp_info_length;
int client_control_socket;

/* Buffer for reply string */
int reply_size;
char reply_string[REPLY_MAXLENGTH];

/* Structures needed for measuring time intervals */
struct timeval time_start, time_now, time_delta;

namespace mbm {

    struct all_args {
        sockaddr_in server_listener_address;
        Config mbm_args;
    };

    void Run(all_args all_args) {

        // create socket
        scoped_ptr<Socket> client_control_socket(new Socket());
        client_control_socket->bindOrDie(DEFAULT_PORT);
        client_control_socket->connectOrDie(all_args.server_listener_address);

        // send serialized config to server
        Packet config_packet(all_args.mbm_args);
        client_control_socket->sendOrDie(config_packet);

        // receive port from server
        uint16_t port = ntohs(client_control_socket->receiveOrDie(sizeof(uint16_t)).as<uint16_t>());
        fprintf(stdout, "THIS IS PORT: %d\n", port);

        // create client_mbm_socket, connect to that port


        scoped_ptr<Socket> client_mbm_socket(new Socket());
        client_mbm_socket->bindOrDie(DEFAULT_PORT2);
        client_mbm_socket->connectOrDie(all_args.server_listener_address.sin_addr.s_addr, port);

        // client_control_socket sends READY
        Packet control_ready_packet(READY, strlen(READY));
        client_control_socket->sendOrDie(control_ready_packet);

        // client_mbm_socket sends READY
        Packet mbm_ready_packet(READY, strlen(READY));
        client_mbm_socket->sendOrDie(mbm_ready_packet);


        // test starts now

        const uint32_t bytes_per_chunk = ntohl(client_control_socket->receiveOrDie(sizeof(bytes_per_chunk)).as<uint32_t>());
        const uint32_t max_num_pkt = ntohl(client_control_socket->receiveOrDie(sizeof(max_num_pkt)).as<uint32_t>());
        const uint32_t max_time = ntohl(client_control_socket->receiveOrDie(sizeof(max_time)).as<uint32_t>());
        const uint64_t target_window_size = ntohl(client_control_socket->receiveOrDie(sizeof(target_window_size)).as<uint32_t>());

        // error handle these?

        //fprintf(stdout, "receiving bytes_per_chunk: %d\n", bytes_per_chunk);
        //fprintf(stdout, "receiving max_num_pkt: %d\n", max_num_pkt);
        //fprintf(stdout, "receiving max_time: %d\n", max_time);

        fprintf(stdout, "receiving at most %d packets (%d bytes)\n", max_num_pkt, max_num_pkt*bytes_per_chunk);
        fprintf(stdout, "the process takes at most %d seconds\n", max_time);
        fprintf(stdout, "target window size is %u packets\n", target_window_size);


        /// client_mbm_socket set max pacing rate (linux only)
        unsigned int rate = 5; //target_window_size;
        printf("Socket pacing set to %u\n", rate);
        if (setsockopt(client_mbm_socket->fd(), IPPROTO_TCP, SO_MAX_PACING_RATE, &rate, sizeof(rate)) < 0) {
            printf("Unable to set socket pacing, using application pacing instead");
        }


        while (true) {
            client_mbm_socket->receiveOrDie(bytes_per_chunk);

            struct tcp_info tcp_info;           
            int tcp_info_length = sizeof(tcp_info);
            static char tcp_options_text[MAX_TCPOPT];
            unsigned short opt_debug = 0;

            if ( getsockopt( client_mbm_socket->fd(), IPPROTO_TCP, TCP_INFO, (void *)&tcp_info, (socklen_t *)&tcp_info_length ) == 0 ) {
                memset((void *)tcp_options_text, 0, MAX_TCPOPT);

                fprintf(stdout, "\n~~~~TCP INFO~~~~\n");

                fprintf(stdout, "tcpi_snd_mss: %u\n", tcp_info.tcpi_snd_mss);
                fprintf(stdout, "tcpi_rcv_mss: %u\n", tcp_info.tcpi_rcv_mss);
                fprintf(stdout, "tcpi_advmss: %u\n", tcp_info.tcpi_advmss); 
                fprintf(stdout, "tcpi_rtt: %u\n", tcp_info.tcpi_rtt);   /* Smoothed RTT in usecs. */
                fprintf(stdout, "tcpi_rttvar: %u\n", tcp_info.tcpi_rttvar); /* RTT variance in usecs. */
                fprintf(stdout, "tcpi_snd_cwnd: %u\n", tcp_info.tcpi_snd_cwnd); /* Send congestion window. */
                fprintf(stdout, "tcpi_rcv_space: %u\n", tcp_info.tcpi_rcv_space);  /* Advertised recv window. */
                fprintf(stdout, "SND RATE Mb/s: %u\n", tcp_info.tcpi_snd_cwnd * tcp_info.tcpi_snd_mss * 8 / tcp_info.tcpi_rtt);
                fprintf(stdout, "RCV RATE: %u\n", tcp_info.tcpi_rcv_space * tcp_info.tcpi_rcv_mss * 8 / tcp_info.tcpi_rtt);

                fprintf(stdout, "tcpi_lost: %u\n", tcp_info.tcpi_lost);
                fprintf(stdout, "tcpi_retrans: %u\n", tcp_info.tcpi_retrans);
                fprintf(stdout, "tcpi_retransmits: %u\n", tcp_info.tcpi_retransmits);

                //fprintf(stdout, "\n~~~~times~~~~\n");
                //fprintf(stdout, "tcpi_last_data_sent: %u\n", tcp_info.tcpi_last_data_sent);
                //fprintf(stdout, "tcpi_last_ack_sent: %u\n", tcp_info.tcpi_last_ack_sent);
                //fprintf(stdout, "tcpi_last_data_recv: %u\n", tcp_info.tcpi_last_data_recv);
                //fprintf(stdout, "tcpi_last_ack_recv: %u\n", tcp_info.tcpi_last_ack_recv);

                //fprintf(stdout, "\n~~~~metrics~~~~\n");
                //fprintf(stdout, "tcpi_pmtu: %u\n", tcp_info.tcpi_pmtu);
                //fprintf(stdout, "tcpi_rcv_ssthresh: %u\n", tcp_info.tcpi_rcv_ssthresh);
                //fprintf(stdout, "tcpi_snd_ssthresh: %u\n", tcp_info.tcpi_snd_ssthresh);
                //fprintf(stdout, "tcpi_reordering: %u\n", tcp_info.tcpi_reordering);

                /* Write some statistics and start of connection to log file. */
                //fprintf(stdout,"# TCP INFO STATS (AdvMSS %u, PMTU %u, options (%0.X): %s)\n",
                //        tcp_info.tcpi_advmss,
                //        tcp_info.tcpi_pmtu,
                //        tcp_info.tcpi_options,
                //        tcp_options_text
                //       );
            }
        }

    }
}	// namespace mbm


mbm::all_args mbm_parse_arguments(int argc, char* argv[]) {

    mbm::all_args new_args;
    mbm::Config new_config;
    new_args.server_listener_address.sin_family = AF_INET;

    static struct option long_opts[] = 
    {
        {"server_address", required_argument, 0, 'a'}, // TODO: 0???
        {"server_port", required_argument, 0, 'p'},        
        {"rate", required_argument, 0, 'r'},
        {"rtt", required_argument, 0, 't'},
        {"mss", required_argument, 0, 'm'},
        {"burst_size", required_argument, 0, 'b'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "a:p:r:t:m:b:", long_opts, &opt_index)) != -1) {
        switch (opt) {
            case 'a':
                inet_pton(AF_INET, optarg, &new_args.server_listener_address.sin_addr);
                break;
            case 'p':
                new_args.server_listener_address.sin_port = htons(atoi(optarg)); // TODO: type check
                //fprintf(stdout, "option -a with value '%d'\n", htons(atoi(optarg)));
                //fprintf(stdout, "set to be: %d\n", new_args.server_listener_address.sin_port);
                break;
            case 'r':
                new_config.rate = atoi(optarg);
                break;
            case 't':
                new_config.rtt = atoi(optarg);
                break;
            case 'm':
                new_config.mss = atoi(optarg);
                break;
            case 'b':
                new_config.burst_size = atoi(optarg);
                break;  
            case '?':
                fprintf(stdout, "ERROR: invalid arguments");
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);                            
        }
    }

    new_args.mbm_args = new_config;
    return new_args;
}




int main(int argc, char* argv[]) {

    /* Get options */
    mbm::all_args all_args = mbm_parse_arguments(argc, argv);
    // fprintf(stdout, "port '%d'", all_args.server_listener_address.sin_port);	
	// buffer
	// daemon
	// debug level
	// logging filename
	// h help
	// port


	// open logs and files...

	// run
	mbm::Run(all_args);

	return 0;	
}