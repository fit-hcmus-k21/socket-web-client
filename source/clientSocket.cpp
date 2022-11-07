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

void clientSocket::downloadFile(char *fileName) {
    int iResult;
    memset(recvbuf, 0, DEFAULT_BUFLEN);
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    // khai báo biến để lưu header, body
    char *header = (char *)malloc(DEFAULT_BUFLEN);
    memset(header, 0, DEFAULT_BUFLEN);
    char *body = (char *)malloc(DEFAULT_BUFLEN);
    memset(body, '\0', sizeof(body));

    // tách header và body
    splitResponse(recvbuf, header, body);
    iResult = iResult - (body - recvbuf);

    // tìm trong header xem có chunked hay không
    if (strstr(header, "chunked") != NULL) {
        downloadFileChunked(fileName, body, iResult);
    } else {
        // tìm trong header để lấy size content
        char *contentLength = (char *) malloc(1024);
        memset(contentLength, 0, 1024);

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

        downloadFileCLength(fileName, body, iResult, length);
    }
}

int clientSocket::downloadFileCLength( char *fileName, char *recvbuf, int iResult, int length)
{
    FILE *f = fopen(fileName, "wb");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    // mở file để ghi
    fwrite(recvbuf, 1, iResult, f);
    length -= iResult;

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

int clientSocket::downloadFileChunked( char *fileName, char *recvbuf, int iResult) {
    FILE *f = fopen(fileName, "wb");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    // khai báo biến lưu chunk size và con trỏ tới chunk data
    char *chunkSize = (char *)malloc(1024);
    char *chunkData = (char *)malloc(DEFAULT_BUFLEN);
    memset(chunkSize, '\0', 1024);
    memset(chunkData, '\0', sizeof(DEFAULT_BUFLEN));

    int data = 0;
    bool crlfShortage = 0;
    int size = 0;

    while (iResult > 0 && size != -1) {
        do {
            if (size == 0) {
                // Nếu data != 0 :
                if (data != 0) {
                    if (crlfShortage) {
                        strcat(chunkData, recvbuf);
                        strcpy(recvbuf, chunkData);
                        recvbuf += 2;
                        if (data == 2) {
                            iResult -= 2;
                            data = 0;
                        } else {
                            iResult -= 1;
                            data = 0;
                        }
                        crlfShortage = false;
                        memset(chunkData, '\0', sizeof(chunkData));
                    } else {
                        strcat(chunkData, recvbuf);
                        strcpy(recvbuf, chunkData);
                        iResult += data;
                        data = 0;
                        memset(chunkData, '\0', sizeof(chunkData));
                    }
                }

                // tìm vị trí của \r\n
                char *t = strstr(recvbuf, "\r\n");
                if (t != NULL) {
                    strncpy(chunkSize, recvbuf, t - recvbuf);

                    // bỏ qua chunk extension cách bởi dấu ; nếu có
                    char *s = strstr(chunkSize, ";");
                    if (s != NULL) {
                        strncpy(chunkSize, chunkSize, s - chunkSize);
                    }
                    size = strtol(chunkSize, NULL, 16);
                    memset(chunkSize, '\0', 1024);
                    if (size == 0) {
                        size = -1;
                        break;
                    }
                    t += 2;
                    iResult = iResult - (t - recvbuf);
                    strcpy(recvbuf, t);

                    if (iResult >= size) {
                        fwrite(recvbuf, 1, size, f);
                        iResult = iResult - size;
                        if (iResult >= 2) {
                            iResult = iResult - 2;
                            strcpy(recvbuf, recvbuf + size + 2);
                        } else {
                            crlfShortage = true;
                            strcpy(chunkData, recvbuf + size);
                            data = iResult;
                            iResult = 0;
                        }
                        size = 0;
                    } else {
                        fwrite(recvbuf, 1, iResult, f);
                        size -= iResult;
                        memset(recvbuf, '\0', DEFAULT_BUFLEN);
                        iResult = 0;
                    }
                } else { // nếu size = 0 mà không tìm thấy \r\n
                    strcpy(chunkData, recvbuf);
                    data = iResult;
                    memset(recvbuf, '\0', DEFAULT_BUFLEN);
                    break;
                }
            } else { // size > 0
                if (iResult >= size) {
                    fwrite(recvbuf, 1, size, f);
                    iResult = iResult - size;
                    if (iResult >= 2) {
                        iResult = iResult - 2;
                        strcpy(recvbuf, recvbuf + size + 2);
                    } else {
                        crlfShortage = true;
                        strcpy(chunkData, recvbuf + size);
                        data = iResult;
                        iResult = 0;
                    }
                    size = 0;
                } else {
                    fwrite(recvbuf, 1, iResult, f);
                    size -= iResult;
                    memset(recvbuf, '\0', DEFAULT_BUFLEN);
                    iResult = 0;
                }
            }

        } while (iResult > 0 ); 

        if (size != -1) {
            memset(recvbuf, '\0', DEFAULT_BUFLEN);
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        }
    }

    fclose(f);
    return 225;   
}

int clientSocket::downloadFolder(char *folderName)
{
    // tạo thư mục
    if (mkdir(folderName) == -1) 
    {
        printf("Error creating directory!\n");
        exit(1);
    }

    // tải file index.html
    char *fileName = (char *)malloc(1024);
    memset(fileName, '\0', 1024);
    strcpy(fileName, folderName);
    strcat(fileName, "\\index.html");

    downloadFile(fileName);


    
    return 225;
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