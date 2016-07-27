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

#include "main.h"

namespace mbm {

    void* ServerThread(void* client_control_socket_ptr) {
        intptr_t client_control_socket = (intptr_t)(client_control_socket_ptr);
        fprintf(stdout, "SERVER THREAD: accepted fd %d\n", client_control_socket);

        char buffer[256];
        int n;


        // TODO: receive config. deserialize it.

        // TODO: create server_mbm_socket (try to pick a port)

        // TODO: send port to client_control_socket

        // TODO: server_mbm_socket accept connection from client_mbm_socket

        // TODO:  client_control_socket receive READY

        // TODO:  client_mbm_socket receive READY         

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
    int server_listener_socket, client_control_socket;
    struct sockaddr_in server_listener_addr, client_control_addr;

    int portno;

    server_listener_socket = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP?
    if (server_listener_socket < 0) {
        fprintf(stdout, "ERROR creating socket");
        exit(EXIT_FAILURE);
    }

    portno = DEFAULT_PORT;
    server_listener_addr.sin_family = AF_INET;
    server_listener_addr.sin_addr.s_addr = INADDR_ANY;
    server_listener_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(server_listener_socket, (struct sockaddr *) &server_listener_addr, sizeof(server_listener_addr)) < 0) {
        fprintf(stdout, "ERROR binding socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }


    listen(server_listener_socket, 5);

    fprintf(stdout, "now listening on address %d, port %d\n", server_listener_addr.sin_addr.s_addr, ntohs(server_listener_addr.sin_port));

    int size_client_control_addr = sizeof(client_control_addr);

    while (true) {
        // TODO: be able to accept multiple connections, by using select() 
        
        client_control_socket = accept(server_listener_socket, (struct sockaddr *)&client_control_addr, (socklen_t *)&size_client_control_addr);
        
        fprintf(stdout, "accepted fd %d\n", client_control_socket);
        
        if (client_control_socket < 0) {
            fprintf(stdout, "ERROR on accepting socket");
            exit(EXIT_FAILURE);
        }


        fprintf(stdout, "creating thread...");

        // Each server socket runs on a different thread.
        pthread_t thread;
        
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
        //                     void *(*start_routine) (void *), void *arg);
        int rc = pthread_create(&thread, NULL, mbm::ServerThread, (void*)client_control_socket);
        if (rc != 0) {
            fprintf(stdout, "ERROR creating thread" ); 
            return 1;
        }
    }

   pthread_exit(NULL);   
   return 0;
}