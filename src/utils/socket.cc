#include "socket.h"

namespace mbm {

	Socket::Socket() {
        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( fd < 0 ) {
            fprintf(stdout, "ERROR creating socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }		
	}

	void bind(sockaddr_in addr) {
        fprintf(stdout, "binding client socket\n");

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            fprintf(stdout, "ERROR binding socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
	}
	
	void connect(sockaddr_in addr) {

        fprintf(stdout, "connecting to %d\n", addr.sin_port);

        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            fprintf(stdout, "FAILED TO CONNECT: %s", strerror(errno));          
            exit(EXIT_FAILURE);
        }

	}


}