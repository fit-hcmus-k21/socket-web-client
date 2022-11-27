#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>

#define DEFAULT_BUFLEN 220503
#define DEFAULT_PORT "80"


// class clientSocket
class clientSocket
{
public:
    // Initialize Winsock
    clientSocket();

    // destructure 
    ~clientSocket();

    // Connect to server
    void connectToServer(char *serverName); 

    // handle request
    void handleRequest(char *host, char *path, char *fileName, char *folderName, char *dir);

    // create request
    char *createRequest(char *host, char *path);   

    // Send an initial buffer
    int sendRequest(char *request);

    // handling errors
    void handleErrorReceiving(int err);

    // download file
    void downloadFile(char *fileName, char *host, char *path);

    // download and save file type content-length
    int downloadFileCLength(char *fileName, int iResult, int length);

    // download and save file type chunked
    int downloadFileChunked( char *fileName, int iResult);

    // download all file of folder
    int downloadFolder(char *folderName, char *host, char *path);

    // multiple request
    int multipleRequest(char ** links, int linkCount, char *host, char *path, char *folderName);

    // Close the socket
    void closeConnection();

private:
    WSADATA wsaData;
    SOCKET ConnectSocket;

    struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;

    char *recvbuf = (char *)malloc(DEFAULT_BUFLEN);
};

