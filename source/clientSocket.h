#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>

#define DEFAULT_BUFLEN 50
#define DEFAULT_PORT "80"


// class clientSocket
class clientSocket {
    public:
        // Initialize Winsock
        clientSocket();

        // destructure 
        ~clientSocket();

        // get server name
        void getServerName(char *host);

        // Connect to server
        void connectToServer(); 

        // handle request
        void handleRequest( char *path, char *fileName, char *folderName, char *dir);

        // create request
        char *createRequest(char *path);   

        // Send an initial buffer
        int sendRequest(char *request);

        // handling errors
        void handleErrorReceiving(int err);

        // download file
        void downloadFile(char *fileName, char *path);

        // download and save file type content-length
        int downloadFileCLength(char *fileName, int length);

        // download and save file type chunked
        int downloadFileChunked( char *fileName);

        // download all file of folder
        int downloadFolder(char *folderName, char *path);

        // multiple request
        int multipleRequest(char ** links, int linkCount, char *path, char *folderName);

        // check if connection is closed
        bool isConnectionClosed();

        // Close the socket
        void closeConnection();

    private:
        WSADATA wsaData;
        SOCKET ConnectSocket;

        struct addrinfo *result;
        struct addrinfo *ptr;
        struct addrinfo hints;

        char *serverName = (char *) malloc(100);
        bool isKeepAlive = true;

        int iResult;
        char *recvbuf = (char *) malloc(DEFAULT_BUFLEN);
};

