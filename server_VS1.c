#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXLINE 8192 /* Max text line length */
#define LISTENQ 1024 /* Second argument to listen() */
#define BUFSIZE 1024

int open_socket(void);
void clientHandler (int clntSock);

static const int MAXCONNECTIONS  = 3;

int main (int argc, char *argv[]){

// Testing for correct number of arguments
if (argc !=2)
    printf("Usage: ./%s <PORT>\n", argv[0]);

// This is the second argument and also port this server will be listening on.
in_port_t servport = atoi(argv[1]);

//Create the socket to listen for connections
int servSock;
if ((servSock = open_socket()) < 0)
    perror("Socket creation failed");

//Create the server address structure
struct sockaddr_in servAddress;                                     //address structure
memset(&servAddress, 0, sizeof(servAddress));                       //set the structure to 0.  (bzero is deprecated DO NOT USE)
servAddress.sin_family = AF_INET; 
servAddress.sin_addr.s_addr = htonl(INADDR_ANY);                                //Address family ipv4 or ipv6
//inet_pton(AF_INET, "127.0.0.1", &servAddress.sin_addr.s_addr);                   //listening on loopback
servAddress.sin_port = htons(servport);                             //Set the port on network byte order (BiG Endian)

//Bind the socket to the local server address
if (bind(servSock, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0)
{
    perror("Failed binding the address...");
}

//Allow the socket to listen for connections
if (listen(servSock, MAXCONNECTIONS) < 0) 
{
    perror("Failed setting up listener...");
}

//Infinite loop to accept connections
for(;;)
{
    struct sockaddr_in clientAddress;                               //Client Address struture for each new connection
    socklen_t clientlen = sizeof(clientAddress);                    //Get the lenght of the client address structure for later use

    //Waiting for client connections
    int clntSock = accept(servSock, (struct sockaddr *)&clientAddress, &clientlen);
    if (clntSock < 0) {
        perror("Failed accepting the cconection...");
    }
    
    //Should be connected at this point

    char clientName[INET_ADDRSTRLEN];                               // Array to hold the string of the client address once converted by ntop
    if (inet_ntop(AF_INET, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL) {
        printf("Handling client %s/%d\n", clientName, ntohs(clientAddress.sin_port));
    }

    clientHandler(clntSock);
    
}

return 0;

}

int open_socket() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket(): failed\n");
    }
    
    printf("Socket created!\n");

    //TODO make socket reusable (man setsockopt, man 7 socket)   ---NOT SURE WHAT THIS MEANS
   
   return sockfd;
   
}

void clientHandler (int clntSock){

    //Echo string buffer I/O
    char buffer[BUFSIZE];

    //Receive message from the client
    ssize_t numBytesRcvd = recv(clntSock, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0)
    {
        perror("Receiveng data failed");
    }

    //Send in a while loop, recheck at the end if there is more to receive and resend
    while (numBytesRcvd > 0){

    ssize_t numBytesSent = send(clntSock, buffer, BUFSIZE, 0);
    if (numBytesSent = 0)
    {
        perror("Sending data failed");
    }

    else if (numBytesSent != numBytesRcvd)
    {
        printf("Sent: Unexpected number of bytes.");            //OK, and now what??
    }
    
    ssize_t numBytesRcvd = recv(clntSock, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0)
    {
        perror("Receiveng data failed");
    }

    }

    close(clntSock);

}



