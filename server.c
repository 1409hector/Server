#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int open_listenfd(int port) {
    int listenfd;
    int optval = 1;

    struct sockaddr_in serveraddr;

    //Create a socket file descriptor
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

    //Set socket options to associate with listenfd
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    //Zero out the serveraddr struct
    bzero((char *)&serveraddr, sizeof(serveraddr));

    //Set internet protocol
    serveraddr.sin_family = AF_INET;

    //Allow incoming connections from any IP
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Indicate port number
    serveraddr.sin_port = htons((unsigned short) port);

    //Bind file descriptor to the socket options struct
    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    //listen on the port and get ready to accept requests
    listen(listenfd, 1024);

    return listenfd;

}

int main(int argc, char *argv[]) {
    int listenfd, connfd, port;

    struct sockaddr_in clientaddr;

    socklen_t clientlen;

    struct hostent *hp;

    char *haddrp;

    unsigned short client_port;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
    }

    //Convert argument 1 to port
    port = atoi(argv[1]);

    //Open listening socket
    listenfd = open_listenfd(port);

    printf("Server is listening on port %d\n", port);

    while(1) {

        clientlen = sizeof(clientaddr);

        //Accept connection
        connfd = accept(listenfd, (struct sockaddr *)(&clientaddr), &clientlen);

        //Determine domain name and IP address of client
        hp = gethostbyaddr((const char *)(&clientaddr.sin_addr.s_addr),
                           sizeof(clientaddr.sin_addr.s_addr), AF_INET);

        //Get connected client's IP address
        haddrp = inet_ntoa(clientaddr.sin_addr);

        //Get connected client's port
        client_port = ntohs(clientaddr.sin_port);

        //Print client info
        printf("Server recieved a connection to %s on IP %s on port %u and has ephemeral port %u\n",hp->h_name, haddrp, port, client_port);

        printf("%d\n",connfd);

        close(connfd);

    }

}