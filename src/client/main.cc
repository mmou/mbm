#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <syslog.h>

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

}	// namespace mbm

int main(int argc, char* argv[]) {
    /* Options with their defaults */
    unsigned short opt_buffer = DEFAULT_BUFFER;
    unsigned short opt_daemon = 0;
    unsigned short opt_debug = 0;
    char *opt_filename = NULL;
    unsigned short opt_port = DEFAULT_PORT;
    unsigned short opt_reply = 0;
    int option;
    /* Program logic */
    unsigned short debug_counter = DEFAULT_LOOPS;
    int client_length;
    int recv_bytes;
    int status;
    static char tcp_options_text[MAX_TCPOPT];
    /* Our process ID and Session ID */
    pid_t pid, sid;


    /* Get options */

	// see https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
	int FLAG_rate = 0;
	int FLAG_rtt = 0;
	int FLAG_mss = 0;
	int FLAG_burst_size = 0;
	
	// buffer
	// daemon
	// debug level
	// logging filename
	// h help
	// port

	opterr = 0
	while ((c = getopt(argc, argv, "rate:rtt:mss:burst:")) != -1) {
		switch (c) {
			case 'rate':
				FLAG_rate = optarg;
				break;
			case 'rtt':
				FLAG_rtt = optarg;
				break;
			case 'mss':
				FLAG_mss = optarg;
				break;
			case 'burst':
				FLAG_burst_size = optarg;
				break;	
			case '?':
				fprintf(stderr, "ERROR: invalid arguments");
				exit(EXIT_FAILURE)
			default:
				abort();							
		}
	}

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