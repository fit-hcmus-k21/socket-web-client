#include "library.h"

clientSocket::clientSocket()
{
    ConnectSocket = INVALID_SOCKET;

    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(225);
    }
}

clientSocket::~clientSocket()
{
    // free recvbuf
    free(recvbuf);

    if (!isClosed) {
        closeConnection();
    }

    // cleanup
    WSACleanup();
}

int clientSocket::connectToServer(char *serverName)
{
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
    freeaddrinfo(ptr);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // manage the connection
    u_long iMode = 1;  // 0 = blocking mode, 1 = non-blocking mode
    ioctlsocket(ConnectSocket, FIONBIO, &iMode); // set non-blocking mode so that the program will not hang

    return 0;
}

void clientSocket::handleRequest(char *host, char *path, char *fileName, char *folderName, char *dir) {
    // nếu là file 
    if (strlen(fileName) > 0) {
        // chỉnh tên file theo cấu trúc: host_fileName, lưu vào dir releases
        char *newFileName = createNewFName(fileName, host, dir);

        downloadFile(newFileName, host, path);

    } else {
        // nếu là folder: chỉnh tên folder theo cấu trúc: host_folderName, lưu vào dir releases
        char *newFolderName = createNewFName(folderName, host, dir);

        downloadFolder(newFolderName, host, path);
    }
}


char *clientSocket::createRequest(char *host, char *path) {
    // Send an initial buffer
    int iResult;
    char *request = (char *)malloc(1024);
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: %s\r\n\r\n", path, host, "keep-alive");
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

void clientSocket::downloadFile(char *fileName, char *host, char *path) {

    char *request = (char *)malloc(100);
    request = createRequest(host, path);
    sendRequest(request);

    printf("Host: %s\n", host);
    printf("File: %s\n", fileName);
    printf("request: %s\n", request);

    int iResult;
    memset(recvbuf, 0, DEFAULT_BUFLEN);
    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

    // khai báo biến để lưu header, body
    char *header = (char *)malloc(DEFAULT_BUFLEN);
    memset(header, 0, DEFAULT_BUFLEN);
    char *body = (char *)malloc(DEFAULT_BUFLEN);
    memset(body, '\0', sizeof(body));

    // tách header và body
    splitResponse(recvbuf, header, body);
    iResult = iResult - (body - recvbuf);

    // kiểm tra connection close hay keep-alive
    if (strstr(header, "Connection: close") != NULL) {
        isKeepAlive = false;
    } else {
        isKeepAlive = true;
    }

    // nếu header có cả transfer encoding và content length thì content length sẽ bị ignore
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
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        fwrite(recvbuf, 1, iResult, f);
        length -= iResult;
    }

    // đóng file
    fclose(f);

    // nếu response từ server có connection close thì đóng connection
    if (isKeepAlive) {
        isClosed = false;
    } else {
        closeConnection();
        isClosed = true;
    }
        
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
    memset(chunkData, '\0', DEFAULT_BUFLEN);

    int data = 0;
    bool crlfShortage = false;
    int size = 0;

    while (iResult > 0 && size != -1) {
        do {
            if (size == 0) {
                // Nếu data != 0 :
                if (data != 0) {
                    if (crlfShortage) {
                        if (data == 1 ) {
                            recvbuf += 1;
                            iResult -= 1;
                        } else {
                            recvbuf += 2;
                            iResult -= 2;
                        }
                        crlfShortage = false;
                    } else {
                        strcat(chunkData, recvbuf);
                        memmove(recvbuf, chunkData, iResult + data);
                        iResult += data;
                        data = 0;
                    }
                }

                // tìm vị trí của \r\n
                char *t = strstr(recvbuf, "\r\n");
                if (t != NULL) {
                    memset(chunkSize, '\0', 1024);
                    strncpy(chunkSize, recvbuf, t - recvbuf);

                    // bỏ qua chunk extension cách bởi dấu ; nếu có
                    char *s = strstr(chunkSize, ";");
                    if (s != NULL) {
                        strncpy(chunkSize, chunkSize, s - chunkSize);
                    }
                    size = strtol(chunkSize, NULL, 16);
                    if (size == 0) {
                        size = -1;
                        cout << "\nsize == -1\n";
                        break;
                    }
                    t += 2;
                    
                    iResult = iResult - (t - recvbuf);
                    recvbuf = t;

                    if (iResult >= size) {
                        fwrite(recvbuf, 1, size, f);
                        iResult = iResult - size;
                        if (iResult >= 2) {
                            iResult = iResult - 2;
                            memcpy(recvbuf, recvbuf + size + 2, iResult);
                        } else {
                            crlfShortage = true;
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
                    memset(chunkData, '\0', DEFAULT_BUFLEN);
                    memmove(chunkData, recvbuf, iResult);
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
                        recvbuf += size + 2;
                    } else {
                        crlfShortage = true;
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
            iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        }
    }

    fclose(f);
    
    // nếu response từ server có connection close thì đóng connection
    if (isKeepAlive) {
        isClosed = false;
    } else {
        closeConnection();
        isClosed = true;
    }

    return 225;   
}

int clientSocket::downloadFolder(char *folderName, char *host, char *path)
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

    downloadFile(fileName, host, path);

    // lọc các link trong file index.html và lưu vào vector link
    vector<char *> links;
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        printf("Error opening file! \n");
        exit(1);
    }

    char *line = (char *)malloc(1024);
    memset(line, '\0', 1024);

    // tìm trong file index.html các link có dạng <a href="link">...</a>
    while (fgets(line, 1024, f) != NULL) {
        char *t = strstr(line, "<a href=\"");
        if (t != NULL) {
            t += 9;
            char *s = strstr(t, "\">");
            if (s != NULL) {
                char *linkName = (char *)malloc(1024);
                memset(linkName, '\0', 1024);
                strncpy(linkName, t, s - t);
                
                // nếu link là file
                if (strstr(linkName, ".") != NULL) {
                    links.push_back(linkName);
                }
            }
        }
    }

    // multiple requests cho các requests tải các file trong folder
    multipleRequest(links, host, path, folderName);

    //  đóng file
    fclose(f);
    
    return 225;
}

int clientSocket::multipleRequest(vector <char*> links, char *host, char *path, char *folderName) {
        
        // tạo request đến các trang trong links để tải file
        for (int i = 0; i < links.size(); i++) {
            char *fileName = (char *)malloc(1024);
            memset(fileName, '\0', 1024);
            strcpy(fileName, folderName);
            strcat(fileName, "\\");
            strcat(fileName, links[i]);

            // tạo đường dẫn tới file
            char *newPath = (char *)malloc(1024);
            memset(newPath, '\0', 1024);
            strcpy(newPath, path);
            strcat(newPath, links[i]);

            // kiểm tra liệu connection đã đóng hay chưa, nếu đã đóng ở lần request trước rồi thì kết nối lại
            if (isClosed) {
                connectToServer(host);
                isClosed = false;
            }

            downloadFile(fileName, host, newPath);
    }
 
}

void clientSocket::closeConnection()
{
    // shutdown the connection since no more data will be sent
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        exit(225);
    }

    // close socket
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET; 

}


