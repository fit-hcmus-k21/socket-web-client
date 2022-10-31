
#define _WIN32_WINNT 0x0501 
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "80"


// class clientSocket
class clientSocket
{
public:
    clientSocket();
    ~clientSocket();

    // Initialize Winsock
    int init(); 

    // Connect to server
    int connectToServer(char *serverName); 

    // create request
    char *createRequest(char *host, char *path);   

    // Send an initial buffer
    int sendRequest(char *request);

    // Receive until the peer closes the connection
    int receiveResponse();

    // download and save file type content-length
    int downloadFile(char *serverName, char *fileName);

    // download and save file type chunked
    int downloadFileChunked(char *serverName, char *fileName);

    // download all file of folder
    int downloadFolder(char *serverName, char *folderName);

    // multiple request
    int multipleRequest(char *serverName, char *fileName);

    // multiple connection
    int multipleConnection(char *serverName, char *fileName);

    // Close the socket
    int closeConnection();

private:
    WSADATA wsaData;
    SOCKET ConnectSocket;
    struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen;
};

