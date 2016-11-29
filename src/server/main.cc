#include "main.h"

#include "cbr.h"
#include "model.h"


#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <errno.h>
#include <iostream>

#include "utils/config.h"
#include "utils/constants.h"
#include "utils/scoped_ptr.h"
#include "utils/socket.h"

#include <csignal>

namespace mbm {

    void* ServerThread(void* client_control_socket_ptr) {
        const Socket* client_control_socket = static_cast<const Socket*>(client_control_socket_ptr);
        //const scoped_ptr<Socket> client_control_socket(reinterpret_cast<Socket*>(client_control_socket_ptr));
        fprintf(stdout, "SERVER THREAD: accepted %d\n", client_control_socket->fd());

        // receive config. deserialize it.
        const Config config = client_control_socket->receiveOrDie(sizeof(Config)).as<Config>();

        // create server_mbm_socket (try to pick a port)
        scoped_ptr<Socket> server_mbm_socket(new Socket());

        int enable = 1;
        if (setsockopt(server_mbm_socket->fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            perror("setsockopt(SO_REUSEADDR) failed");
        }

        server_mbm_socket->bindOrDie(BASE_PORT);

        // server_mbm_socket accept connection from client_mbm_socket
        server_mbm_socket->listenOrDie();


        /// server_mbm_socket set max pacing rate (linux only)
        //int64_t target_window_size = model::target_window_size(config.rate, config.rtt, config.mss); // todo: probably shouldn't call this here
        unsigned int rate = config.rate * 1000 / 8; // kilobits per second --> bytes per second
        printf("Socket pacing set to %u\n", rate);
        if (setsockopt(server_mbm_socket->fd(), SOL_SOCKET, SO_MAX_PACING_RATE, &rate, sizeof(rate)) < 0) {
           printf("Unable to set socket pacing, using application pacing instead:%s", strerror(errno));
        } else {
            printf("attempt successful, rate is %u\n", rate);
        }

        // send port to client_control_socket
        Packet port_packet(htons(BASE_PORT));
        client_control_socket->sendOrDie(port_packet);
        fprintf(stdout, "THIS IS PORT: %d\n", BASE_PORT);

        // server_mbm_socket accept connection from client_mbm_socket
        const mbm::Socket* client_mbm_socket(server_mbm_socket->acceptOrDie());

        // client_control_socket receive READY
        // ssize_t num_bytes;
        std::string control_ready = (client_control_socket->receiveOrDie(strlen(READY))).str();
        fprintf(stdout, "control received %s\n", control_ready.c_str());

        // client_mbm_socket receive READY         
        std::string mbm_ready = (client_mbm_socket->receiveOrDie(strlen(READY))).str();
        fprintf(stdout, "mbm received %s\n", mbm_ready.c_str());

        //std::raise(SIGINT);
        RunCBR(client_mbm_socket, client_control_socket, config);
       
        pthread_exit(NULL);

    }

} // namespace mbm

int main( int argc, char *argv[] ) {
    struct sockaddr_in server_listener_addr;

    // create socket
    scoped_ptr<mbm::Socket> server_listener_socket(new mbm::Socket());

    int enable = 1;
    if (setsockopt(server_listener_socket->fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    server_listener_socket->bindOrDie(DEFAULT_PORT);
    server_listener_socket->listenOrDie();

    while (true) {
        // TODO: be able to accept multiple connections, by using select() 

        const mbm::Socket* client_control_socket(server_listener_socket->acceptOrDie());
        
        // Each server socket runs on a different thread.
        pthread_t thread;
        int fd = client_control_socket->fd();
        fprintf(stdout, "main THREAD: fd %d\n", fd);

        int rc = pthread_create(&thread, NULL, mbm::ServerThread, (void *) client_control_socket);
        if (rc != 0) {
            fprintf(stdout, "ERROR creating thread" ); 
            return 1;
        }
    }

   pthread_exit(NULL);   
   return 0;
}
