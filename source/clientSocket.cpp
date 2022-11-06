#include "library.h"

clientSocket::clientSocket()
{
    ConnectSocket = INVALID_SOCKET;
    recvbuflen = DEFAULT_BUFLEN;
}

clientSocket::~clientSocket()
{
}

int clientSocket::init() 
{
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    return 0;
}

int clientSocket::connectToServer(char *serverName)
{
    // manage the connection
    u_long iMode = 1;  // 0 = blocking mode, 1 = non-blocking mode
    ioctlsocket(ConnectSocket, FIONBIO, &iMode); // set non-blocking mode so that the program will not hang

    int iResult;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port

    iResult = getaddrinfo(serverName, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    return 0;
}

char *clientSocket::createRequest(char *host, char *path) {
    // Send an initial buffer
    int iResult;
    char *request = (char *)malloc(1024);
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    return request;
}

int clientSocket::sendRequest(char *request)
{
    int iResult;
    // Send an initial buffer
    iResult = send( ConnectSocket, request, (int)strlen(request), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);
    return 0;
}

int clientSocket::downloadFile( char *fileName)
{
    FILE *f = fopen(fileName, "wb");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    int iResult;
    memset(recvbuf, '\0', sizeof(recvbuf));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    // printf("\nDATA: \n%s\n", recvbuf);

    // khai báo biến để lưu header, body, content-type
    char *header = (char *)malloc(DEFAULT_BUFLEN);
    char *body = (char *)malloc(DEFAULT_BUFLEN);
    memset(body, '\0', sizeof(body));
    char *contentLength = (char *)malloc(1024);
    char *contentType = (char *)malloc(1024);

    // tách header, body, content-Type từ response
    splitResponse(recvbuf, header, body, contentType);

     // tìm vị trí của content-length
    char *s = strstr(header, "Content-Length: ");
    if (s != NULL) {
        // tách content-length
        contentLength = s + 16;
        // tìm vị trí của \r\n
        char *t = strstr(contentLength, "\r\n");
        if (t != NULL) {
            strncpy(contentLength, contentLength, t - contentLength);
        }
    } else {
        s = strstr(header, "content-length: ");
        if (s != NULL) {
            // tách content-length
            contentLength = s + 16;
            // tìm vị trí của \r\n
            char *t = strstr(contentLength, "\r\n");
            if (t != NULL) {
                strncpy(contentLength, contentLength, t - contentLength);
            }
        }
    }

    // Chuyển content-length từ string sang int
    int length = atoi(contentLength);

    // mở file để ghi
    int n = iResult - (body - recvbuf);
    fwrite(body, 1, n, f);
    length -= n;

    // tiếp tục nhận dữ liệu
    while (length > 0 && iResult > 0) {
        memset(recvbuf, '\0', DEFAULT_BUFLEN); // xóa dữ liệu trong buffer
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        fwrite(recvbuf, 1, iResult, f);
        length -= iResult;
    }

    // đóng file
    fclose(f);
        
    return 0;
    
}

int clientSocket::downloadFileChunked( char *fileName) {
    FILE *f;
    f = fopen(fileName, "w");

    int iResult;
    memset(recvbuf, '\0', sizeof(recvbuf));
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    // khai báo biến để lưu header, body, content-type, content-length
    char *header = (char *)malloc(DEFAULT_BUFLEN);
    char *body = (char *)malloc(DEFAULT_BUFLEN);
    memset(body, '\0', sizeof(body));
    char *contentLength = (char *)malloc(1024);
    char *contentType = (char *)malloc(1024);

    // tách header, body, content-type từ response
    splitResponse(recvbuf, header, body, contentType);

    // in dữ liệu để kiểm tra
    printf("\n\nHEADER: \n\n%s\n\n", header);
    printf("\n\nBODY: \n\n%s\n\n", body);

    // dộ dài của chunk data
    int length = -1;

    char *chunkLength = (char *)malloc(1024);
    char *chunkData = (char *)malloc(DEFAULT_BUFLEN);
    memset(chunkData, '\0', sizeof(chunkData));

    strcpy(recvbuf, body);
    char *p = strstr(recvbuf, "\r\n");
    do {
        while (p != NULL) {
            if (length == 0 || length == -1) {
                strncpy(chunkLength, recvbuf, p - recvbuf);
                // Tìm và bỏ chunk extension nếu có
                char *t = strstr(chunkLength, ";");
                if (t != NULL) {
                    strncpy(chunkLength, chunkLength, t - chunkLength);
                }
                // chuyển chunkLength từ hexa sang dec
                length = strtol(chunkLength, NULL, 16);
                
                printf("\n\nLength: %d\n\n",length);

                if (length == 0) {
                    length = -1;
                    break;
                }
                recvbuf = p + 2;
                if (strlen(recvbuf) >= length + 2) {
                    strncpy(chunkData, recvbuf, length);
                    fwrite(chunkData, 1, length, f);
                    memset(chunkData, '\0', sizeof(chunkData));
                    length = 0;
                    recvbuf = recvbuf + length + 2;
                    p = strstr(recvbuf, "\r\n");
                } else {
                    fwrite(recvbuf, 1, strlen(recvbuf), f);
                    length -= strlen(recvbuf);
                    memset(recvbuf, '\0', sizeof(recvbuf));
                    p = NULL;
                }
            } else {
                if (p - recvbuf < length && strlen(recvbuf) >= length + 2) {
                    strncpy(chunkData, recvbuf, length);
                    fwrite(chunkData, 1, length, f);
                    memset(chunkData, '\0', sizeof(chunkData));
                    length = 0;
                    recvbuf = recvbuf + length + 2;
                    p = strstr(recvbuf, "\r\n");
                } if (strlen(recvbuf) < length) {
                    fwrite(recvbuf, 1, strlen(recvbuf), f);
                    length -= strlen(recvbuf);
                    memset(recvbuf, '\0', sizeof(recvbuf));
                    p = NULL;
                } else {
                    fwrite(recvbuf, 1, length, f);
                    length = 0;
                    recvbuf = recvbuf + length + 2;
                    p = strstr(recvbuf, "\r\n");
                }
                
            }
        }
        if (p == NULL && length > 0) {
            
                printf("\n\nLength: %d\n\n",length);

                // ghi file
                fwrite(recvbuf, 1, strlen(recvbuf), f);
                memset(recvbuf, '\0', sizeof(recvbuf));
                length -= iResult;        
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        p = strstr(recvbuf, "\r\n");
        printf("\n\nDATA: %s\n", recvbuf);
    } while (iResult > 0 && length != -1);

    fclose(f);
    return 225;   
}

int clientSocket::downloadFolder(char *folderName)
{
    
}

int clientSocket::multipleRequest(char *serverName, char *fileName)
{
    
}

int clientSocket::multipleConnection(char *serverName, char *fileName)
{
    
}


int clientSocket::closeConnection()
{
    // shutdown the connection since no more data will be sent
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}