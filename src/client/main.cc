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

//#include "common/packet.h"

#include "main.h"

// const char default_filename[] = DEFAULT_FILENAME;

/* File descriptor for log file */
// FILE *statistics;

/* Things we need for dealing with TCP */
struct sockaddr_in client_ctrl_address, server_ctrl_address; //client_address;
char *tcp_buffer;
struct tcp_info tcp_info;
int tcp_info_length;
int ctrl_socket;
int tcp_work_socket;

/* Buffer for reply string */
int reply_size;
char reply_string[REPLY_MAXLENGTH];

/* Structures needed for measuring time intervals */
struct timeval time_start, time_now, time_delta;

namespace mbm {

    struct mbm_config {
        int rate;
        int rtt;
        int mss;
        int burst_size;
    };

    struct all_args {
        sockaddr_in server_ctrl_address;
        mbm_config mbm_args;
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


        /*


        connect to port: create a new socket based on config, mbm_socket 

        ctrl_socket: send ready

        mbm_socket: set receive, send timeouts

        ctrl_socket: set timeout to be 3*rtt for the ready-ack loop

        mbm_socket: send ready, until receive ready on ctrl_socket

        ---- test should start now
        Server drives the test by picking a CBR and sending data at that rate while counting losses. All we need to do is receive and dump the data.

        ctr_socket receives 
            chunk_len
            max_num_pkt
            max_time_sec

        while true:
            if ctrl_socket receives END, break
            if mbm_socket receives Packet recv, push TrafficData of recv onto list data_collected

        ctrl_socket: send data_collected  to server
            send data_collected.size()
            send each TrafficData/Packet

        ctrl_socket: receive result
        */



        // ctrl_socket: 
            // set receive, send timeouts

            // send config over ctrl_socket

            // receive port

        // 





        fprintf(stdout, "creating client socket\n");

        /* Prepare TCP socket. */
        // int socket(int domain, int type, int protocol);
        ctrl_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( ctrl_socket < 0 ) {
            /* Could not open socket. */
            syslog( LOG_DAEMON | LOG_CRIT, "Could not open TCP socket: %s", strerror(errno) );
            exit(EXIT_FAILURE);
        }

        /* Bind to any address on local machine */
        // The bind() function assigns an address to an unnamed socket.
        client_ctrl_address.sin_family = AF_INET;
        client_ctrl_address.sin_addr.s_addr = INADDR_ANY;
        client_ctrl_address.sin_port = htons(opt_port);

        fprintf(stdout, "binding client socket\n");

        if (bind(ctrl_socket, (struct sockaddr *)&client_ctrl_address, sizeof(client_ctrl_address)) < 0) {
            /* Cannot bind to socket. */
            syslog( LOG_DAEMON | LOG_CRIT, "Cannot bind to socket: %s", strerror(errno) );
            exit(EXIT_FAILURE);
        }

        /* We can now listen for incoming connections. We only allow a backlog of one
         * connection
         */
        // listen - listen for socket connections and limit the queue of incoming connections
        //if (listen(ctrl_socket, 1) != 0) {
        //    syslog( LOG_DAEMON | LOG_CRIT, "Cannot listen on socket: %s", strerror(errno) );
        //    exit(EXIT_FAILURE);
        //}


        fprintf(stdout, "connecting to %d\n", all_args.server_ctrl_address.sin_port);
        //connect to server
        // int connect(int socket, const struct sockaddr *address, socklen_t address_len);
        if (connect(ctrl_socket, (struct sockaddr *)&all_args.server_ctrl_address, sizeof(all_args.server_ctrl_address)) != 0) {
            fprintf(stdout, "FAILED TO CONNECT: %s", strerror(errno));          
        }


        char buffer[256];
        int  n;
   
        printf("Please enter the message: ");
           bzero(buffer,256);
           fgets(buffer,255,stdin);
           
           /* Send message to the server */
           //n = write(ctrl_socket, buffer, strlen(buffer));

           n = send(ctrl_socket, buffer, strlen(buffer), 0);
           
           if (n < 0) {
              perror("ERROR writing to socket");
              exit(1);
           }
           
           /* Now read server response */
           bzero(buffer,256);
           n = recv(ctrl_socket, buffer, 255, 0);
           
           if (n < 0) {
              perror("ERROR reading from socket");
              exit(1);
           }
            
           printf("%s\n",buffer);


        // send config to server
        // ssize_t send(int socket, const void *buffer, size_t length, int flags);
        ///// send(all_args.server_ctrl_address.sin_port, )


        // receive test port from server
        // <<<<<<<<<<<<


        //// why? what is this buffer for?????
        ///* Allocate Buffer for TCP stream data.
        // * (We store it temporarily only since we act as an TCP sink.)
        // */
        //tcp_buffer = malloc(opt_buffer);
        //if ( tcp_buffer == NULL ) {
        //    syslog( LOG_DAEMON | LOG_CRIT, "Can't allocate buffer for TCP temporary memory.\n" );
        //    exit(EXIT_FAILURE);
        //}
//
//
        //////////
        //// ssize_t recv(int socket, void *buffer, size_t length, int flags);
        ///* recv_port = recv(ctrl_socket, tcp_buffer, opt_buffer, 0); */
        /////////
//
//
        //client_length = sizeof(client_address);
        //tcp_work_socket = accept( tcp_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_length );
        ///* Get time for counting milliseconds. */
//
//
        ///* As soon as we got a connection, we deal with the incoming data by using
        // * a second socket. We only read as much as opt_buffer bytes.
        // */
        //if ( (recv_bytes = recv( tcp_work_socket, tcp_buffer, opt_buffer, 0 ) ) > 0 ) {
        //    /* Fill tcp_info structure with data to get the TCP options and the client's
        //     * name.
        //     */
        //    tcp_info_length = sizeof(tcp_info);
        //    if ( getsockopt( tcp_work_socket, SOL_IP, TCP_INFO, (void *)&tcp_info, (socklen_t *)&tcp_info_length ) == 0 ) {
        //        memset((void *)tcp_options_text, 0, MAX_TCPOPT);
        //        decode_tcp_options(tcp_options_text,tcp_info.tcpi_options);
        //        if ( opt_debug > 0 ) {
        //            printf("Got a new connection from client %s.\n",inet_ntoa(client_address.sin_addr));
        //        }
        //        else {
        //            syslog( LOG_DAEMON | LOG_INFO, "Received connection from client at address %s.",
        //                   inet_ntoa(client_address.sin_addr));
        //        }
        //        /* Write some statistics and start of connection to log file. */
        //        fprintf(statistics,"# Received connection from %s (AdvMSS %u, PMTU %u, options (%0.X): %s)\n",
        //                inet_ntoa(client_address.sin_addr),
        //                tcp_info.tcpi_advmss,
        //                tcp_info.tcpi_pmtu,
        //                tcp_info.tcpi_options,
        //                tcp_options_text
        //               );
        //    }
        //}
        //while ( (recv_bytes = recv( tcp_work_socket, tcp_buffer, opt_buffer, 0 ) ) > 0 ) {
        //    if ( opt_debug > 0 ) {
        //        printf("\nReceived %d bytes on socket.\n",recv_bytes);
        //    }
        //    /* Measure time in order to create time intervals. */
        //    get_now( &time_now, opt_debug );
        //    /* Fill tcp_info structure with data */
        //    tcp_info_length = sizeof(tcp_info);
//
        //    if ( getsockopt( tcp_work_socket, SOL_TCP, TCP_INFO, (void *)&tcp_info, (socklen_t *)&tcp_info_length ) == 0 ) {
        //        if ( opt_debug > 0 ) {
        //            printf("snd_cwnd: %u\nsnd_ssthresh: %u\nrcv_ssthresh: %u\nrtt: %u\nrtt_var: %u\n",
        //                   tcp_info.tcpi_snd_cwnd,
        //                   tcp_info.tcpi_snd_ssthresh,
        //                   tcp_info.tcpi_rcv_ssthresh,
        //                   tcp_info.tcpi_rtt,
        //                   tcp_info.tcpi_rttvar
        //                  );
        //        }
        //        fprintf(statistics,"%.6f %u %u %u %u %u %u %u %u %u %u %u %u\n",
        //                time_to_seconds( &time_start, &time_now ),
        //                tcp_info.tcpi_last_data_sent,
        //                tcp_info.tcpi_last_data_recv,
        //                tcp_info.tcpi_snd_cwnd,
        //                tcp_info.tcpi_snd_ssthresh,
        //                tcp_info.tcpi_rcv_ssthresh,
        //                tcp_info.tcpi_rtt,
        //                tcp_info.tcpi_rttvar,
        //                tcp_info.tcpi_unacked,
        //                tcp_info.tcpi_sacked,
        //                tcp_info.tcpi_lost,
        //                tcp_info.tcpi_retrans,
        //                tcp_info.tcpi_fackets
        //               );
        //        if ( fflush(statistics) != 0 ) {
        //            if ( opt_debug > 0 ) {
        //                fprintf(stderr, "Cannot flush buffers: %s\n", strerror(errno) );
        //            }
        //            else {
        //                syslog( LOG_DAEMON | LOG_CRIT, "Cannot flush buffers: %s", strerror(errno) );
        //            }
        //        }
        //        /* Send reply text via TCP connection */
        //        if ( opt_reply != 0 ) {
        //            reply_size = snprintf( reply_string, REPLY_MAXLENGTH, "rcv_ssthresh %u\n", tcp_info.tcpi_rcv_ssthresh );
        //            if ( reply_size > 0 ) {
        //                if ( send( tcp_work_socket, (void *)reply_string, reply_size, MSG_DONTWAIT ) == -1 ) {
        //                    if ( opt_debug > 0 ) {
        //                        fprintf(stderr, "Reply size %u didn't match: %s\n", reply_size, strerror(errno) );
        //                    }
        //                    else {
        //                        syslog( LOG_DAEMON | LOG_ERR, "Reply size %u didn't match: %s", reply_size, strerror(errno) );
        //                    }
        //                }
        //            }
        //        }
        //    }
        //}
        //close(tcp_work_socket);
        //fprintf(statistics,"# Closed connection from %s.\n",inet_ntoa(client_address.sin_addr));
        //if ( fflush(statistics) != 0 ) {
        //    if ( opt_debug > 0 ) {
        //        fprintf(stderr, "Cannot flush buffers: %s\n", strerror(errno) );
        //    }
        //    else {
        //        syslog( LOG_DAEMON | LOG_CRIT, "Cannot flush buffers: %s", strerror(errno) );
        //    }
        //}
        //if ( opt_debug > 0 ) {
        //    debug_counter--;
        //    printf("Closed connection. Decrementing debug counter to %u.\n\n",debug_counter);
        //}
        //else {
        //    syslog( LOG_DAEMON | LOG_INFO, "Closed connection from %s",
        //           inet_ntoa(client_address.sin_addr));
        //}
        //sleep(DEFAULT_SLEEP);
//
        ///* That's a happy ending. */
        //exit(EXIT_SUCCESS);



    
    }
}	// namespace mbm


mbm::all_args mbm_parse_arguments(int argc, char* argv[]) {

    // see https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
    // see https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html

    mbm::all_args new_args;

    new_args.server_ctrl_address.sin_family = AF_INET;

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

    //fprintf(stdout, "hielloe!");

    int opt;
    int opt_index = 0;
    while ((opt = getopt_long(argc, argv, "a:p:r:t:m:b:", long_opts, &opt_index)) != -1) {
        switch (opt) {
            case 'a':
                inet_aton(optarg, &new_args.server_ctrl_address.sin_addr);
                break;
            case 'p':
                new_args.server_ctrl_address.sin_port = htons(atoi(optarg)); // TODO: type check
                //fprintf(stdout, "option -a with value '%d'\n", htons(atoi(optarg)));
                //fprintf(stdout, "set to be: %d\n", new_args.server_ctrl_address.sin_port);
                break;
            case 'r':
                new_args.mbm_args.rate = atoi(optarg);

                break;
            case 't':
                new_args.mbm_args.rtt = atoi(optarg);

                break;
            case 'm':
                new_args.mbm_args.mss = atoi(optarg);

                break;
            case 'b':
                new_args.mbm_args.burst_size = atoi(optarg);

                break;  
            case '?':
                fprintf(stdout, "ERROR: invalid arguments");
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);                            
        }
    }
    return new_args;
}




int main(int argc, char* argv[]) {

    /* Get options */
    mbm::all_args all_args = mbm_parse_arguments(argc, argv);
    // fprintf(stdout, "port '%d'", all_args.server_ctrl_address.sin_port);	
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