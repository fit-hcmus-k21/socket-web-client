#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>

#define DEFAULT_BUFLEN 305022
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
        bool connectToServer(); 

        // handle request
        bool handleRequest( char *path, char *fileName, char *folderName, char *dir);

        // create request
        void createRequest(char *path, char *request);   

        // Send an initial buffer
        bool sendRequest(char *request);

        // handle errors
        void handleError(int err);

        // download file
        bool downloadFile(char *fileName, char *path);

        // download and save file type content-length
        bool downloadFileCLength(char *fileName, int length);

        // download and save file type chunked
        bool downloadFileChunked( char *fileName);

        // download all file of folder
        bool downloadFolder(char *folderName, char *path);

        // multiple request
        bool multipleRequest(char ** links, int linkCount, char *path, char *folderName);

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
        int recvbuflen = DEFAULT_BUFLEN;
        char *recvbuf ;
};

