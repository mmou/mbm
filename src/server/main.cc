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

#include "main.h"

namespace mbm {

    void* ServerThread(void* client_control_socket_ptr) {
        scoped_ptr<Socket> client_control_socket(reinterpret_cast<Socket*>(client_control_socket_ptr));
        fprintf(stdout, "SERVER THREAD: accepted %d\n", client_control_socket->fd());

        // receive config. deserialize it.
        const Config config = client_control_socket->receiveOrDie(sizeof(Config)).as<Config>();

        // create server_mbm_socket (try to pick a port)
        scoped_ptr<Socket> server_mbm_socket(new Socket());
        server_mbm_socket->bindOrDie(BASE_PORT);

        // server_mbm_socket accept connection from client_mbm_socket
        server_mbm_socket->listenOrDie();

        // send port to client_control_socket
        Packet port_packet(htons(BASE_PORT));
        client_control_socket->sendOrDie(port_packet);
        fprintf(stdout, "THIS IS PORT: %d\n", BASE_PORT);

        // server_mbm_socket accept connection from client_mbm_socket
        const mbm::Socket* client_mbm_socket(server_mbm_socket->acceptOrDie());


        uint16_t port = ntohs(client_control_socket->receiveOrDie(sizeof(uint16_t)).as<uint16_t>());
        fprintf(stdout, "THIS IS PORT: %d\n", port);


        // client_control_socket receive READY
        ssize_t num_bytes;
        std::string control_ready = (client_control_socket->receiveOrDie(num_bytes)).str();

        // client_mbm_socket receive READY         
        std::string mbm_ready = (client_mbm_socket->receiveOrDie(num_bytes)).str();



        // TODO: RunCBR(client_mbm_socket, client_control_socket, config)


        //////////////////

//        /* If connection is established then start communicating */
//        bzero(buffer,256);
//        if (recv( client_control_socket,buffer,255, 0) < 0) {
//           perror("SERVER THREAD: ERROR reading from socket");
//           exit(EXIT_FAILURE);
//        }
//       
//        printf("SERVER THREAD: Here is the message: %s\n",buffer);
//       
//        /* Write a response to the client */        
//        if (send(client_control_socket,"I got your message",18, 0) < 0) {
//            perror("ERROR writing to socket");
//            exit(EXIT_FAILURE);
//        }
        
        pthread_exit(NULL);

    }

} // namespace mbm

int main( int argc, char *argv[] ) {
    struct sockaddr_in server_listener_addr;

    // create socket
    scoped_ptr<mbm::Socket> server_listener_socket(new mbm::Socket());
    server_listener_socket->bindOrDie(DEFAULT_PORT);
    server_listener_socket->listenOrDie();

    while (true) {
        // TODO: be able to accept multiple connections, by using select() 

        const mbm::Socket* client_control_socket(server_listener_socket->acceptOrDie());

        // Each server socket runs on a different thread.
        pthread_t thread;
        int fd = client_control_socket->fd();
        fprintf(stdout, "main THREAD: fd %d\n", fd);

        int rc = pthread_create(&thread, NULL, mbm::ServerThread, (void*)client_control_socket);
        if (rc != 0) {
            fprintf(stdout, "ERROR creating thread" ); 
            return 1;
        }
    }

   pthread_exit(NULL);   
   return 0;
}