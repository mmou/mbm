#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <getopt.h>

#include <errno.h>
#include <string.h>

#include <syslog.h>

#include "main.h"

// const char default_filename[] = DEFAULT_FILENAME;

/* File descriptor for log file */
// FILE *statistics;

/* Things we need for dealing with TCP */
struct sockaddr_in server_address,client_address;
char *tcp_buffer;
struct tcp_info tcp_info;
int tcp_info_length;
int tcp_socket;
int tcp_work_socket;

/* Buffer for reply string */
int reply_size;
char reply_string[REPLY_MAXLENGTH];

/* Structures needed for measuring time intervals */
struct timeval time_start, time_now, time_delta;

namespace mbm {

    struct mbm_args {
        int rate;
        int rtt;
        int mss;
        int burst_size;
    };


}	// namespace mbm


mbm::mbm_args mbm_parse_arguments(int argc, char **argv) {

    // see https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
    // see https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html

    mbm::mbm_args new_args;

    static struct option long_opts[] = 
    {
        {"rate", required_argument, 0, 'r'},
        {"rtt", required_argument, 0, 't'},
        {"mss", required_argument, 0, 'd'},
        {"burst_size", required_argument, 0, 'c'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "r:t:m:b:", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'r':
                new_args.rate = atoi(optarg);
                break;
            case 't':
                new_args.rtt = atoi(optarg);
                break;
            case 'm':
                new_args.mss = atoi(optarg);
                break;
            case 'b':
                new_args.burst_size = atoi(optarg);
                break;  
            case '?':
                fprintf(stderr, "ERROR: invalid arguments");
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);                            
        }
    }

    return new_args;
}




int main(int argc, char* argv[]) {
    /* Options with their defaults */
    unsigned short opt_buffer = DEFAULT_BUFFER;
    unsigned short opt_daemon = 0;
    unsigned short opt_debug = 0;
    char *opt_filename = NULL;
    unsigned short opt_port = DEFAULT_PORT;
    unsigned short opt_reply = 0;
    /* Program logic */
    unsigned short debug_counter = DEFAULT_LOOPS;
    int client_length;
    int recv_bytes;
    int status;
    static char tcp_options_text[MAX_TCPOPT];
    /* Our process ID and Session ID */
    pid_t pid, sid;


    /* Get options */
	
	// buffer
	// daemon
	// debug level
	// logging filename
	// h help
	// port



	// open logs and files...

    /* Prepare TCP socket. */
	// int socket(int domain, int type, int protocol);
    tcp_socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ); // where
    if ( tcp_socket == -1 ) {
        /* Could not open socket. */
        if ( opt_debug > 0 ) {
            fprintf(stderr,"Could not open TCP socket: %s\n",strerror(errno));
        }
        else {
            syslog( LOG_DAEMON | LOG_CRIT, "Could not open TCP socket: %s", strerror(errno) );
        }
        exit(EXIT_FAILURE);
    }
    else {
        /* Bind to any address on local machine */
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(opt_port);
        memset((void *)&(server_address.sin_zero), '\0', 8);
        status = bind( tcp_socket, (struct sockaddr *)&server_address, sizeof(server_address) );
        if ( status == 0 ) {
            /* We can now listen for incoming connections. We only allow a backlog of one
             * connection
             */
            status = listen( tcp_socket, 1 );

            if ( status != 0 ) {
                /* Cannot listen on socket. */
                if ( opt_debug > 0 ) {
                    fprintf(stderr,"Cannot listen on socket: %s\n",strerror(errno));
                }
                else {
                    syslog( LOG_DAEMON | LOG_CRIT, "Cannot listen on socket: %s", strerror(errno) );
                }
                exit(EXIT_FAILURE);
            }
        }
        else {
            /* Cannot bind to socket. */
            if ( opt_debug > 0 ) {
                fprintf(stderr,"Cannot bind to socket: %s\n",strerror(errno));
            }
            else {
                syslog( LOG_DAEMON | LOG_CRIT, "Cannot bind to socket: %s", strerror(errno) );
            }
            exit(EXIT_FAILURE);
        }
    }




	// run
//	mbm::Run(FLAG_rate, FLAG_rtt, FLAG_mss, FLAG_burst_size);

	return 0;	
}