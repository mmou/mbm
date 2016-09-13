#ifndef SOCKET_H
#define SOCKET_H

#include "packet.h"

namespace mbm {

	class Socket {
		public:
			Socket();
			Socket(int fd);

			int fd() const;

			void bindOrDie(unsigned short port);
			void bindOrDie(const sockaddr_in &addr);

			void listenOrDie();
			Socket* acceptOrDie();

			void connectOrDie(unsigned short port);
			void connectOrDie(unsigned long address, unsigned short port);			
			void connectOrDie(const sockaddr_in &addr);

			void sendOrDie(Packet packet);
			Packet receiveOrDie(size_t size);

		private:
			int fd_;

	};
	
}

#endif  // SOCKET_H
