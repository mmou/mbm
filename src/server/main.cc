#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "main.h"

/*

main
   ListenSocket socket
   while true:
      accept new conections: AcceptedSocket ctrl_socket
      ServerConfig
      create new thread (mlab pthread_t) for each new server socket



ServerThread
   get available port
   AcceptedSocket ctrl_socket
      set receive, send timeout

ctrl_socket receive config

create ListenSocket listen_socket on some port


scoped_ptr<ListenSocket> mbm_socket(listen_socket) // what is this doing

ctrl_socket: tell client to connect to listen port

accept test connection
AcceptedSocket test_socket_buff / test_socket

AcceptedSocket test_socket
   test_socket: set receive, send timeout


ctrl_socket receive READY
test_socket receive READY

RunCBR(test_socket, ctrl_socket)




*/


int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n;
   
   // fprintf(stdout, "creating socket\n");
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP?

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
   //serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   // fprintf(stdout, "binding socket\n");
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   fprintf(stdout, "now listening on address %d, port %d\n", serv_addr.sin_addr.s_addr, ntohs(serv_addr.sin_port));

   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	
   fprintf(stdout, "accepted fd %d\n", newsockfd);

   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

   /* If connection is established then start communicating */
   bzero(buffer,256);
   n = recv( newsockfd,buffer,255, 0);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   printf("Here is the message: %s\n",buffer);
   
   /* Write a response to the client */
   n = send(newsockfd,"I got your message",18, 0);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
      
   return 0;
}