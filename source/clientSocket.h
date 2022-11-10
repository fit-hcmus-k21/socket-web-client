
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

    // handle request
    void handleRequest(char *host, char *path, char *fileName, char *folderName, char *dir);

    // create request
    char *createRequest(char *host, char *path);   

    // Send an initial buffer
    int sendRequest(char *request);

    // download file
    void downloadFile(char *fileName, char *host, char *path);

    // download and save file type content-length
    int downloadFileCLength(char *fileName, char *recvbuf, int iResult, int length, bool isKeepAlive);

    // download and save file type chunked
    int downloadFileChunked( char *fileName, char *recvbuf, int iResult, bool isKeepAlive);

    // download all file of folder
    int downloadFolder(char *folderName, char *host, char *path);

    // multiple request
    int multipleRequest(vector <char*> links, char *host, char *path);

    // multiple connection
    int multipleConnection(char *serverName, char *fileName);

    // Close the socket
    int closeConnection();

    // check connection is closed
    bool isConnectionClosed();

private:
    WSADATA wsaData;
    SOCKET ConnectSocket;

    struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;

    char *recvbuf = (char *)malloc(DEFAULT_BUFLEN);
    int recvbuflen;

    bool isKeepAlive;
    bool isClosed;
};

