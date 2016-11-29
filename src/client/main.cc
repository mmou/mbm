#include "main.h"

#include "utils/config.h"
#include "utils/constants.h"
#include "utils/scoped_ptr.h"
#include "utils/socket.h"
#include "utils/result.h"

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <sys/poll.h>

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
        int enable = 1;

        // create socket
        scoped_ptr<Socket> client_control_socket(new Socket());

        if (setsockopt(client_control_socket->fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
        }

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

        if (setsockopt(client_mbm_socket->fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
        }

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


        // start test
        while (true) {

            // receive test packets
            struct pollfd ufds[2];
            int rv;

            ufds[0].fd = client_control_socket->fd();
            ufds[0].events = POLLIN; // | POLLPRI; // check for normal or out-of-band

            ufds[1].fd = client_mbm_socket->fd();
            ufds[1].events = POLLIN; // check for just normal data

            rv = poll(ufds, 2, max_time+120);

            if (rv == -1) {
                perror("poll"); // error occurred in poll()
                break;
            } else if (rv == 0) {
                fprintf(stdout, "Max time reached.\n");
                break;
            } else {
                // check for events on client_control_socket:
                if (ufds[0].revents & POLLIN) {
                    std::string control_end = (client_control_socket->receiveOrDie(strlen(END))).str();
                    fprintf(stdout, "control received %s\n", control_end.c_str());
                    break;
                }

                // check for events on client_mbm_socket:
                if (ufds[1].revents & POLLIN) {
                    if (client_mbm_socket->receiveOrDie(bytes_per_chunk).length() < bytes_per_chunk) {
                        fprintf(stdout, "Something went wrong. The server might have died: %s\n", strerror(errno));
                        break;
                    } else {
                        // packet received
                    }
                }
            }
        }

        Result test_result = (client_control_socket->receiveOrDie(sizeof(Result))).as<Result>();
        fprintf(stdout, "client hears the TEST RESULT: %s\n", kResultStr[test_result]);

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
