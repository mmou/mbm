#include "socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


namespace mbm {

	Socket::Socket() {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // IPPROTO_TCP or 0?
        if ( fd_ < 0 ) {
            fprintf(stdout, "ERROR creating socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else {
        	fprintf(stdout, "SUCCESS created socket at %d\n", fd_);
        }
	}

	Socket::Socket(int fd) {
        fd_ = fd;
        if ( fd_ < 0 ) {
            fprintf(stdout, "ERROR creating socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else {
        	fprintf(stdout, "SUCCESS created socket at %d\n", fd_);
        }
	}

    int Socket::fd() const {
        return fd_;
    }

	void Socket::bindOrDie(const sockaddr_in& addr) {
        if (bind(fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            fprintf(stdout, "ERROR binding socket on address %d, port %d: %s", addr.sin_addr.s_addr, ntohs(addr.sin_port), strerror(errno));
            exit(EXIT_FAILURE);
        } else {
	        fprintf(stdout, "SUCCESS binding socket on address %d, port %d\n", addr.sin_addr.s_addr, ntohs(addr.sin_port));
        }
	}

	void Socket::bindOrDie(unsigned short port) {
		sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bindOrDie(addr);

	}

	void Socket::listenOrDie() {
	    if (listen(fd_, 1) < 0) {
			fprintf(stdout, "ERROR listening socket: %s", strerror(errno));    	
            exit(EXIT_FAILURE);
	    } else {
		    fprintf(stdout, "SUCCESS now listening\n");
	    }
	}
	
	Socket* Socket::acceptOrDie() {
	    int afd;
		struct sockaddr_in addr;
    	int size_addr = sizeof(addr);

        afd = accept(fd_, (struct sockaddr *)&addr, (socklen_t *)&size_addr);
        
        if (afd < 0) {
            fprintf(stdout, "ERROR on accepting socket");
            exit(EXIT_FAILURE);
        } else {
    	    fprintf(stdout, "SUCCESS accepted fd %d\n", afd);
    	    return new Socket(afd);
        }
	}


	void Socket::connectOrDie(const sockaddr_in& addr) {
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), addr_str, INET_ADDRSTRLEN);

        fprintf(stdout, "attempting connecting to address %s, port %d\n", addr_str, ntohs(addr.sin_port));

        if (connect(fd_, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
            fprintf(stdout, "FAILED TO CONNECT: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else {
	        fprintf(stdout, "SUCCESS connecting to address %s, port %d\n", addr_str, ntohs(addr.sin_port));
        }
	}

	void Socket::connectOrDie(unsigned short port) {
		sockaddr_in addr; 
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        connectOrDie(addr);
	}

    void Socket::connectOrDie(unsigned long address, unsigned short port) {
        sockaddr_in addr; 
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = address;
        addr.sin_port = htons(port);

        connectOrDie(addr);     
    }          


	bool Socket::sendOrDie(Packet packet) const {
        if (send(fd_, packet.buffer(), packet.length(), 0) < 0) {
            fprintf(stdout, "ERROR writing to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
            return false;
        } else {
            fprintf(stdout, "SUCCESS sending packet: %s ; buffer length %d\n", packet.buffer(), packet.length());            
            return true;
        }
	}

	Packet Socket::receiveOrDie(size_t size) const {
        char buffer[size];
        if (recv(fd_, buffer, size, 0) < 0) {
            fprintf(stdout, "ERROR reading from socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            Packet packet(buffer, size);
            fprintf(stdout, "SUCCESS receiving packet: %s ; of size %d buffer length %d\n", packet.buffer(), size, packet.length());            
            return packet;
        }
    }


}