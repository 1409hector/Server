#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFSIZE 1024

int open_socket(void);


int main(int argc, char *argv[]) {
    int clientfd;
    char *serverIP;
    in_port_t serverPort;
    char *echoString;
    char buf[BUFFSIZE];
   
   if (argc < 3 || argc > 4) {
        printf("usage: ./%s <serverIP> <echo string> [<port>]\n", argv[0]);
        exit(-1);
    }
   
    serverIP = argv[1];                                                         //First argument server address string
    echoString = argv[2];                                                       // second argument is the string to be echoed by the server
    
    //Port is an optional parameter; default 7 wellknown echo port
    serverPort = (argc == 4) ? atoi(argv[3]) : 7; 
    
    clientfd = open_socket();

    //Build server address structure
    struct sockaddr_in serveraddr;                                              //Server address structure
    memset(&serveraddr, 0, sizeof(serveraddr));                                 //Set it to 0
    serveraddr.sin_family = AF_INET;

    //Convert the address from string to network address and store it in .s_addr
    int retVal = inet_pton(AF_INET, serverIP, &serveraddr.sin_addr.s_addr);
    if (retVal == 0) {
        perror("inet_pton(): invalid address string\n");
    }
    else if (retVal < 0) {
        perror("inet_pton(): failed\n");
    }

    serveraddr.sin_port = htons(serverPort);                                          //Server port
    
    //Connect the client
    if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        perror("Connect(): failed");
   
    printf("Client connected\n");
   
    size_t echoStringLen = strlen(echoString);

    //Send the string to server
    ssize_t numbytes = send(clientfd, echoString, echoStringLen, 0);
    if (numbytes < 0)
    {
        perror("send(): failed");
    }
    if (numbytes != echoStringLen)
    {
        perror("Sent unexpected number of bytes");              //What do we do if send is less bytes than the lenght we specified
    }

    //Receive the same string back from the server

    unsigned int totalBytesRecvd = 0;           //count of bytes received
    fputs("Received this string: ", stdout);                 //setup to print the echo string

    while (totalBytesRecvd < echoStringLen)
    {
        char buffer[BUFFSIZE];                  //Input output buffer

        //Receive data up to the buffer size minus 1 to allow space for the null byte
        numbytes = recv(clientfd, buffer, BUFFSIZE -1, 0);
        if (numbytes < 0)
        {
            perror("recv(): failed");
        }
        
        if (numbytes == 0)
        {
            perror("Connection closed prematurely");
        }

        totalBytesRecvd += numbytes;        //Tracking bytes received
        buffer[numbytes] = '\0';            //Terminate the string
        fputs(buffer, stdout);              //Print the echo buffer
        
    }
    
        fputc('\n', stdout);
   
    close(clientfd);
    return 0;                               // exit (0)??
   
}

int open_socket() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket(): failed\n");
    }
    
    printf("Socket created!\n");

    //TODO make socket reusable (man setsockopt, man 7 socket)
   
   return sockfd;
   
}