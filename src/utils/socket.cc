#include "socket.h"

namespace mbm {

	Socket::Socket() {
        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( fd < 0 ) {
            fprintf(stdout, "ERROR creating socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }		
	}

	void Socket::bindOrDie(unsigned short port) {
		sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bindOrDie(addr);

	}

	void Socket::bindOrDie(sockaddr_in addr) {
        fprintf(stdout, "binding client socket\n");

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            fprintf(stdout, "ERROR binding socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
	}


	void Socket::connectOrDie(unsigned short port) {
		sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        connectOrDie(addr);
	}
	
	void Socket::connectOrDie(sockaddr_in* addr) {

        if (connect(fd, (struct sockaddr *)&addr, sizeof(&addr)) != 0) {
            fprintf(stdout, "FAILED TO CONNECT: %s", strerror(errno));          
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "connecting to %d\n", addr->sin_port);


	}


	void Socket::sendOrDie(Packet packet) {
        if (send(fd, packet.buffer(), packet.length(), 0) < 0) {
            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
	}


	Packet Socket::receiveOrDie(size_t size) {
        char port_buffer[size];
        if (recv(fd, port_buffer, size, 0) < 0) {
            fprintf(stdout, "ERROR reading from socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        Packet packet(port_buffer, size);
        return packet;
    }


}