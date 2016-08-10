#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "packet.h"

namespace mbm {

	class Socket {
		public:
			Socket();
			void bindOrDie(unsigned short port);
			void bindOrDie(sockaddr_in* addr);

			void connectOrDie(unsigned short port);
			void connectOrDie(sockaddr_in* addr);
			void sendOrDie(Packet packet);
			Packet receiveOrDie(size_t size);

		private:
			int fd;
	};
	
}