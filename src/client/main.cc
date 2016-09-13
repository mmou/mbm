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

        /// client_mbm_socket set max pacing rate (linux only)
        unsigned int rate = 10;
        printf("Socket pacing set to %u\n", rate);
        if (setsockopt(client_mbm_socket->fd(), SOL_SOCKET, SO_MAX_PACING_RATE, &rate, sizeof(rate)) < 0) {
            printf("Unable to set socket pacing, using application pacing instead");
        }


        // client_control_socket sends READY
        Packet control_ready_packet(READY, strlen(READY));
        client_control_socket->sendOrDie(control_ready_packet);

        // client_mbm_socket sends READY
        Packet mbm_ready_packet(READY, strlen(READY));
        client_mbm_socket->sendOrDie(mbm_ready_packet);


        // test starts now

        // ...


//        /* Send message to the server */
//        if (send(client_control_socket, "HELLO SERVER!", 13, 0) < 0) {
//            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
//            exit(EXIT_FAILURE);
//        }
//
//        /* Now read server response */
//        bzero(buffer,256);
//        if (recv(client_control_socket, buffer, 255, 0) < 0) {
//            fprintf(stdout, "ERROR reading from socket: %s", strerror(errno));
//            exit(EXIT_FAILURE);
//        }
//        
//        printf("%s\n",buffer);

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
        {"mss", required_argument, 0, 'd'},
        {"burst_size", required_argument, 0, 'c'},
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