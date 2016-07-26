#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "main.h"

int main( int argc, char *argv[] ) {
    int server_listener_socket, client_control_socket;
    struct sockaddr_in server_listener_addr, client_control_addr;

    int portno;
    char buffer[256];
    int n;

    server_listener_socket = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP?
    if (server_listener_socket < 0) {
        fprintf(stdout, "ERROR creating socket");
        exit(1);
    }


    portno = DEFAULT_PORT;
    server_listener_addr.sin_family = AF_INET;
    server_listener_addr.sin_addr.s_addr = INADDR_ANY;
    server_listener_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(server_listener_socket, (struct sockaddr *) &server_listener_addr, sizeof(server_listener_addr)) < 0) {
        fprintf(stdout, "ERROR binding socket");
        exit(1);
    }

    /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
    */
    listen(server_listener_socket, 5);

    fprintf(stdout, "now listening on address %d, port %d\n", server_listener_addr.sin_addr.s_addr, ntohs(server_listener_addr.sin_port));

    int size_client_control_addr = sizeof(client_control_addr);






    client_control_socket = accept(server_listener_socket, (struct sockaddr *)&client_control_addr, (socklen_t *)&size_client_control_addr);
    
    fprintf(stdout, "accepted fd %d\n", client_control_socket);
    
    if (client_control_socket < 0) {
        fprintf(stdout, "ERROR on accepting socket");
        exit(1);
    }


    ///// >>> while true, accept new connections, start new threads....


    //// Each server socket runs on a different thread.
    //pthread_t thread;
    //
    //// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    //                      void *(*start_routine) (void *), void *arg);
    //int rc = pthread_create(&thread, NULL, mbm::ServerThread,
    //                        (void*)server_config);
    //if (rc != 0) {
    //  std::cerr << "Failed to create thread: " << strerror(errno) << " ["
    //            << errno << "]\n";
    //  return 1;



//**   ////>>>>>>>>>
//**   /* Accept actual connection from the client */
//**   client_control_socket = accept(server_listener_socket, (struct sockaddr *)&client_control_addr, (socklen_t *)&size_client_control_addr);
//**	
//**   fprintf(stdout, "accepted fd %d\n", client_control_socket);
//**
//**   if (client_control_socket < 0) {
//**      perror("ERROR on accept");
//**      exit(1);
//**   }
//**
//**   /* If connection is established then start communicating */
//**   bzero(buffer,256);
//**   n = recv( client_control_socket,buffer,255, 0);
//**   
//**   if (n < 0) {
//**      perror("ERROR reading from socket");
//**      exit(1);
//**   }
//**   
//**   printf("Here is the message: %s\n",buffer);
//**   
//**   /* Write a response to the client */
//**   n = send(client_control_socket,"I got your message",18, 0);
//**   
//**   if (n < 0) {
//**      perror("ERROR writing to socket");
//**      exit(1);
//**   }
      
   return 0;
}