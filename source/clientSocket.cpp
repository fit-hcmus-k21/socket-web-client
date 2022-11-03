#include "clientSocket.h"

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

int clientSocket::receiveResponse()
{
    int iResult;
    // Receive until the peer closes the connection
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
        {
            printf("Bytes received: %d\n", iResult);
            printf("%s\n", recvbuf);
        }
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );
    return 0;
}

int clientSocket::downloadFile(char *serverName, char *fileName)
{
    FILE *f;

    // xác định kiểu file
    // char *fileType = strrchr(fileName, '.');
    

    // // nếu là file hình ảnh thì mở file để ghi
    // if (strcmp(fileType, ".jpg") == 0 || strcmp(fileType, ".png") == 0 || strcmp(fileType, ".bmp") == 0)
    // {
    //     f = fopen(fileName, "wb");
    //     if (f == NULL)
    //     {
    //         printf("Error opening file!\n");
    //         exit(1);
    //     }
    // }
    // else
    // {
    //     f = fopen(fileName, "w");
    //     if (f == NULL)
    //     {
    //         printf("Error opening file!\n");
    //         exit(1);
    //     }
    // }
            int iResult;
            memset(recvbuf, 0, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("data: %s\n", recvbuf);

            // tách chuỗi để lấy phần header
            char *header = (char *)malloc(1024);
            char *body = (char *)malloc(DEFAULT_BUFLEN);

            char *p = strstr(recvbuf, "\r\n\r\n");
            strncpy(header, recvbuf, p - recvbuf);
            strcpy(body, p + 4);

            // lấy content length
            char *contentLength = (char *)malloc(1024);
            contentLength = strstr(header, "Content-Length: ");
            contentLength += 16;
            int length = atoi(contentLength);

            // lấy content type
            char *contentType = (char *)malloc(1024);
            contentType = strstr(header, "Content-Type: ");
            printf("content type: %s\n", contentType);


            // tìm trong content-type nếu có text thì mở file để ghi text, nếu có image thì mở file để ghi ảnh
            
            if (strstr(contentType, "image") != NULL || strstr(contentType, "application") != NULL)
            {
                f = fopen(fileName, "wb");
                if (f == NULL)
                {
                    printf("Error opening file!\n");
                    exit(1);
                }
                // ghi nội dung vào file
                printf("ghi file binary\n");
                fwrite(body, 1, strlen(body), f);
                length -= strlen(body);
                // printf("data: %s\n", body);
                printf("length: %d\n", length);

                // đọc tiếp nội dung
                while (length > 0 && iResult > 0){
                    memset(recvbuf, 0, DEFAULT_BUFLEN); // xóa dữ liệu trong buffer
                    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                    fwrite(recvbuf, 1, iResult, f);
                    length -= iResult;
                    // printf("length: %d\n", length);
                    // printf("%s", recvbuf);
                }
    
            } else // if (strstr(contentType, "text") != NULL)
            {
                f = fopen(fileName, "w");
                if (f == NULL)
                {
                    printf("Error opening file!\n");
                    exit(1);
                }
                // ghi nội dung vào file
                fprintf(f, "%s", body);
                // printf("data: %s\n", body);
                // printf("length: %d\n", length);
                // tải nội dung còn lại
                length -= strlen(body);
                printf("length: %d\n", length);
                while (length > 0 && iResult > 0)
                {
                    memset(recvbuf, 0, DEFAULT_BUFLEN); // xóa dữ liệu trong buffer
                    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                    fprintf(f, "%s", recvbuf);
                    length -= iResult;
                    // printf("length: %d\n", length);
                }
            }
    fclose(f);
        
    return 0;
    
}

int clientSocket::downloadFileChunked(char *serverName, char *fileName)
{
    
    
}

int clientSocket::downloadFolder(char *serverName, char *folderName)
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
    return 0;
}