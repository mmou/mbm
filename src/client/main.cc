#include <iostream>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <getopt.h>

#include <errno.h>
#include <string.h>

#include <syslog.h>

#include "utils/config.h"
#include "utils/packet.h"
#include "utils/constants.h"

#include "main.h"

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

        unsigned short opt_port = DEFAULT_PORT;


        /* Options with their defaults */
        unsigned short opt_buffer = DEFAULT_BUFFER;
        unsigned short opt_daemon = 0;
        unsigned short opt_debug = 0;
        char *opt_filename = NULL;
        unsigned short opt_reply = 0;
        /* Program logic */
        unsigned short debug_counter = DEFAULT_LOOPS;
        int client_length;
        int recv_bytes;
        int status;
        static char tcp_options_text[MAX_TCPOPT];
        /* Our process ID and Session ID */
        pid_t pid, sid;

        char buffer[256];

        fprintf(stdout, "creating client socket\n");

        // create socket
        scoped_ptr<Socket> client_control_socket(Socket::create());

        client_control_address.sin_family = AF_INET;
        client_control_address.sin_addr.s_addr = INADDR_ANY;
        client_control_address.sin_port = htons(opt_port);
        client_control_socket->bind(client_control_address);

        client_control_socket->connect(server_listener_address);

        // send serialized config to server
        Packet config_packet(all_args.mbm_args);
        if (send(client_control_socket, config_packet.buffer(), config_packet.length(), 0) < 0) {
            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // receive port from server
        char port_buffer[sizeof(uint16_t)];
        if (recv(client_control_socket, port_buffer, sizeof(uint16_t), 0) < 0) {
            fprintf(stdout, "ERROR reading from socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        Packet port_packet(port_buffer, sizeof(uint16_t));        
        uint16_t port = ntohs(port_packet.as<uint16_t>());

        fprintf(stdout, "THIS IS PORT: %d\n", port);




        // create client_mbm_socket, connect to that port
        /* Prepare TCP socket. */
        int client_mbm_socket;
        struct sockaddr_in client_mbm_address, server_mbm_address;
        client_mbm_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( client_mbm_socket < 0 ) {
            fprintf(stdout, "ERROR creating socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Bind to any address on local machine */
        unsigned short opt_port2 = DEFAULT_PORT2;
        client_mbm_address.sin_family = AF_INET;
        client_mbm_address.sin_addr.s_addr = INADDR_ANY;
        client_mbm_address.sin_port = htons(opt_port2);

        fprintf(stdout, "binding client mbm socket\n");

        if (bind(client_mbm_socket, (struct sockaddr *)&client_mbm_address, sizeof(client_mbm_address)) < 0) {
            fprintf(stdout, "ERROR binding socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Bind to any address on local machine */
        server_mbm_address.sin_family = AF_INET;
        server_mbm_address.sin_addr.s_addr = INADDR_ANY;
        server_mbm_address.sin_port = htons(port);

        fprintf(stdout, "connecting to %d\n", server_mbm_address.sin_port);
        //connect to server
        if (connect(client_mbm_socket, (struct sockaddr *)&server_mbm_address, sizeof(server_mbm_address)) != 0) {
            fprintf(stdout, "FAILED TO CONNECT: %s", strerror(errno));          
            exit(EXIT_FAILURE);
        }


        // client_control_socket sends READY
        Packet control_ready_packet(READY, strlen(READY));
        if (send(client_control_socket, control_ready_packet.buffer(), control_ready_packet.length(), 0) < 0) {
            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // client_mbm_socket sends READY
        Packet mbm_ready_packet(READY, strlen(READY));
        if (send(client_mbm_socket, mbm_ready_packet.buffer(), mbm_ready_packet.length(), 0) < 0) {
            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

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
                inet_aton(optarg, &new_args.server_listener_address.sin_addr);
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