#include "library.h"

clientSocket::clientSocket() {
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: "  << iResult << endl;
        exit(225);
    }
}

clientSocket::~clientSocket() {
    free(serverName);
    free(recvbuf);
    closeConnection();

    // cleanup
    WSACleanup();
    cout << "destructured !" << endl;
}

void clientSocket::getServerName(char *host) {
    strcpy(serverName, host);
}

void clientSocket::connectToServer() {
    cout << "connecting to server" << endl;

    ConnectSocket = INVALID_SOCKET;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;    // 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(serverName, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        exit(225);
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "socket failed with error: " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            WSACleanup();
            exit(225);
        }

        // set the socket to non-blocking
        // u_long iMode = 1;  // 0 = blocking mode, 1 = non-blocking mode
        // ioctlsocket(ConnectSocket, FIONBIO, &iMode); // set non-blocking mode so that the program will not hang

        // if (iResult != 0) {
        //     cout << "ioctlsocket failed with error: " << iResult << endl; 
        //     cout << "Error code : " << WSAGetLastError()  << endl;
        // }       

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
        cout << " Unable to connect to server!\n";
        WSACleanup();
        exit(225);
    }
    cout << "Connected to server ! \n";
    
}

void clientSocket::handleRequest(char *path, char *fileName, char *folderName, char *dir) {
    cout << "handling request" << endl;

    // nếu là file 
    if (strlen(fileName) > 0) {
        // chỉnh tên file theo cấu trúc: host_fileName, lưu vào dir releases
        char *newFileName = (char *) malloc (100);
        createNewFName(fileName, serverName, dir, newFileName);

        downloadFile(newFileName, path);

        free(newFileName);

    } else {
        // nếu là folder: chỉnh tên folder theo cấu trúc: host_folderName, lưu vào dir releases
        char *newFolderName = (char *) malloc(100);
        createNewFName(folderName, serverName, dir, newFolderName);

        downloadFolder(newFolderName, path);

        free(newFolderName);
    }

}


char *clientSocket::createRequest( char *path) {
    cout << "creating request" << endl;

    // Send an initial buffer
    char *request = (char *)malloc(1024);
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: %s\r\n\r\n", path, serverName, "keep-alive");
    return request;
}

int clientSocket::sendRequest(char *request) {
    cout << "sending request" << endl;

    // Send an initial buffer
    iResult = send( ConnectSocket, request, (int)strlen(request), 0 );  
    if (iResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        handleErrorReceiving(err);
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Bytes Sent: " << iResult << endl;
    return 0;
}


void clientSocket::handleErrorReceiving(int err) {

    cout << "handling error  ! " << endl;
    cout << "Error code: " << err << endl;

    switch (err) {
        case WSAECONNREFUSED:
            cout << "Error: connection refused !" << endl;
            cout << "Can call connect again for the same socket ! " << endl;
            break;
        case WSAETIMEDOUT:
            cout << "Error: connection timed out !" << endl;
            cout << "Can call connect again for the same socket ! " << endl;
            break;
        case WSAENETUNREACH:
            cout << "Error: network is unreachable !" << endl;
            cout << "Can call connect again for the same socket ! " << endl;
            break;
        case WSAEWOULDBLOCK:
            cout << "Error: resource temporarily unavailable !" << endl;
            break;
        case WSAENETDOWN:
            cout << "Error: network is down ! " << endl;
            break;
        case WSAENETRESET:
            cout << "Error: the connection has been broken due to keep-alive activity that detected a failure while the operation was in progress !" << endl;
            break;
        case WSAECONNABORTED:
            cout << "Error: software caused connection abort !" << endl;
            break;
        case WSAECONNRESET:
            cout << "Error: connection reset by peer !" << endl;
            break;
        case WSAENOBUFS:
            cout << "Error: no buffer space available !" << endl;
            break;
        case WSAENOTCONN:
            cout << "Error: socket is not connected !" << endl;
            break;
        case WSAESHUTDOWN:
            cout << "Error: cannot send after socket shutdown !" << endl;
            break;
        case WSAEHOSTDOWN:
            cout << "Error: host is down !" << endl;
            break;
        case WSAHOST_NOT_FOUND:
            cout << "Error: host not found !" << endl;
            break;
    }
}

void clientSocket::downloadFile(char *fileName, char *path) {

    cout << "downloading file" << endl;

    char *request;
    request = createRequest( path);
    sendRequest(request);

    cout << "Host: " <<  serverName << endl;
    cout << "File: " << fileName << endl;
    cout << "Request: " << request << endl;

    memset(recvbuf, '\0', DEFAULT_BUFLEN);
    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);


    if (iResult == 0 || iResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        handleErrorReceiving(err);
        if (err == WSAEWOULDBLOCK) {
            Sleep(100);
            iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if (iResult == 0 || iResult == SOCKET_ERROR) {
                cout << "Error receiving data !" << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                exit(225);
            }
            
        } else if (err == WSAETIMEDOUT || err == WSAENETUNREACH || err == WSAECONNREFUSED) {
            cout << "Connect to server again ! " << endl;
            connectToServer();
            downloadFile(fileName, path);
            return;
        } else {
            closesocket(ConnectSocket);
            WSACleanup();
            exit(225);
        }
    }

    // nếu chưa nhận hết header thì tiếp tục nhận *****
    if (strstr(recvbuf, "\r\n\r\n") == NULL) {
        cout << "header not complete" << endl;
        cout << "header: " << recvbuf << endl;

        char *temp = new char [DEFAULT_BUFLEN];

        // tiếp tục nhận đến khi xong header
        while (strstr(recvbuf, "\r\n\r\n") == NULL) {
            int recvBytes = recv(ConnectSocket, temp, DEFAULT_BUFLEN, 0);
            if (recvBytes == 0 || recvBytes == SOCKET_ERROR) {
                int err = WSAGetLastError();
                handleErrorReceiving(err);
                if (err == WSAEWOULDBLOCK) {
                    Sleep(100);
                    iResult += recv(ConnectSocket, temp, DEFAULT_BUFLEN, 0);
                    if (recvBytes == 0 || recvBytes == SOCKET_ERROR) {
                        cout << "Error receiving data !" << endl;
                        closesocket(ConnectSocket);
                        WSACleanup();
                        exit(225);
                    }
                } 
            }
        
            iResult += recvBytes;

            // cấp phát thêm bộ nhớ cho recvbuf để chứa dữ liệu mới nhận được
            recvbuf = (char *)realloc(recvbuf, iResult + 1);

            // nối dữ liệu mới nhận được vào recvbuf
            strncpy(recvbuf + iResult - recvBytes, temp, recvBytes);

            // khởi tạo lại temp
            memset(temp, '\0', DEFAULT_BUFLEN);
        }
        delete [] temp;
    }
    cout << "header: " << recvbuf << endl;

    cout << "Bytes received: " <<  iResult << endl;

    // khai báo biến để lưu header, body
    char *header = (char *) malloc (iResult + 1);
    memset(header, '\0', iResult + 1);
    char *body;

    // tách header và body
    splitResponse(recvbuf, header, body);
    iResult = iResult - (body - recvbuf);
    cout << "header: " << body - header << endl;
    cout << "memove body: " << iResult << endl;
    memmove(recvbuf, body, iResult);
    cout << "recvbuf: " << recvbuf << endl;

    // nếu response header không phải là 200 OK thì thoát
    if (strstr(header, "200 OK") == NULL) {
        cout << "Response header is not 200 OK\n";
        closeConnection();
        return;
    }

    // nếu response có connection close thì nhận xong đóng kết nối (áp dụng trong multi-request)
    if (strstr(header, "Connection: close") != NULL) {
        isKeepAlive = false;
    } else {
        isKeepAlive = true;
    }

    // nếu header có cả transfer encoding và content length thì content length sẽ bị ignore
    // tìm trong header xem có chunked hay không
    if (strstr(header, "chunked") != NULL) {
        downloadFileChunked(fileName);
    } else {
        // tìm trong header để lấy size content
        char *contentLength = (char *) malloc(100);
        memset(contentLength, 0, 100);

         // tìm vị trí của content-length
        char *s = strstr(header, "Content-Length: ");
        if (s != NULL) {
            // tách content-length
            strcpy(contentLength, s + 16);
            // tìm vị trí của \r\n
            char *t = strstr(contentLength, "\r\n");
            if (t != NULL) {
                strncpy(contentLength, contentLength, t - contentLength);
            }
        } else {
            s = strstr(header, "content-length: ");
            if (s != NULL) {
                // tách content-length
                strcpy(contentLength, s + 16);
                // tìm vị trí của \r\n
                char *t = strstr(contentLength, "\r\n");
                if (t != NULL) {
                    strncpy(contentLength, contentLength, t - contentLength);
                }
            }
        }
        // Chuyển content-length từ string sang int
        int length = atoi(contentLength);

        // giải phóng bộ nhớ
        free(contentLength);

        downloadFileCLength(fileName, length);
    }
}

int clientSocket::downloadFileCLength( char *fileName, int length) {
    cout << "downloading file with content length" << endl;

    FILE *f = fopen(fileName, "wb");
    if (f == NULL)
    {
        cout << "Error opening file!\n";
        exit(1);
    }

    cout << "recvbuf: " << recvbuf << endl;
    cout << "length: " << length << endl;
    cout << "iResult: " << iResult << endl;

    // mở file để ghi
    fwrite(recvbuf, 1, iResult, f);
    length -= iResult;

    // tiếp tục nhận dữ liệu
    while (length > 0) {
        memset(recvbuf, '\0', DEFAULT_BUFLEN); // xóa dữ liệu trong buffer
        iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult == SOCKET_ERROR || iResult == 0) {
            int err= WSAGetLastError();
            cout << "recv failed with error: " << err << endl;
            if (err == WSAEWOULDBLOCK) {  // currently no data available
                Sleep(100);  // wait and try again: 100ms
                iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
            } else {
                return 225;
            }
        }
        fwrite(recvbuf, 1, iResult, f);
        length -= iResult;
    }

    // đóng file
    fclose(f);
        
    return 0;
    
}

int clientSocket::downloadFileChunked( char *fileName) {
    cout << "downloading file with chunked" << endl;

    FILE *f = fopen(fileName, "wb");
    if (f == NULL)
    {
        cout << "Error opening file!\n";
        exit(1);
    }

    // khai báo biến lưu chunk size và con trỏ tới chunk data
    char *chunkSize = (char *)malloc(1000);
    memset(chunkSize, '\0', 1000);
    char *chunkData = (char *)malloc(1000);

    int data = 0;
    bool crlfShortage = false;
    int size = 0;

    while (size != -1) {
        do {
            if (size == 0) {  // đọc chunk size
                // Nếu data != 0 thì xảy ra 2 trường hợp:
                if (data != 0) {
                    cout << "crlfShortage: " << crlfShortage << endl;
                    // TH1: thiếu \r\n ở cuối chunk data trước đó
                    if (crlfShortage) {
                        if (data == 1 ) {
                            iResult -= 1;
                            memmove(recvbuf, recvbuf + 1, iResult);
                        } else {
                            iResult -= 2;
                            memmove(recvbuf, recvbuf + 2, iResult);
                        }
                        crlfShortage = false;
                        data = 0;

                    } else { // TH2: chưa tìm thấy cuối chuỗi chunk size ở lần đọc trước

                        // cấp phát thêm bộ nhớ cho chunk data lưu dữ liệu đọc được
                        chunkData = (char *)realloc(chunkData, data + iResult);
                        strncpy(chunkData + data, recvbuf, iResult);

                        // cấp phát thêm bộ nhớ cho recvbuf
                        recvbuf = (char *)realloc(recvbuf, DEFAULT_BUFLEN + iResult);

                        // di chuyển dữ liệu 
                        memmove(recvbuf, chunkData, iResult + data);
                        iResult += data;
                        data = 0;
                        memset(chunkData, '\0', 1000);
                    }
                }

                cout << "du lieu truoc khi tach chunk size: " << recvbuf << endl;

                // tìm vị trí của \r\n
                char *t = strstr(recvbuf, "\r\n");
                if (t != NULL) {
                    memset(chunkSize, '\0', 1000);
                    strncpy(chunkSize, recvbuf, t - recvbuf);

                    // bỏ qua chunk extension cách bởi dấu ; nếu có
                    char *s = strstr(chunkSize, ";");
                    if (s != NULL) {
                        strncpy(chunkSize, chunkSize, s - chunkSize);
                        cout << "has chunk extension" << s << endl;
                    }
                    cout << "chunk size: " << chunkSize << endl;

                    // chuyển chunk size từ hex sang dec
                    size = strtol(chunkSize, NULL, 16);
                    if (size == 0) {
                        size = -1;
                        cout << "\nsize == -1\n";
                        cout << "chunkSize: " << chunkSize << endl;
                        break;
                    }
                    cout << "size = " << size << endl;

                    t += 2;
                    
                    // cập nhật lại iResult
                    iResult = iResult - (t - recvbuf);
                    memmove(recvbuf, t, iResult);
                    
                    // nếu iResult >= size thì ghi file và cập nhật lại size
                    if (iResult >= size) {
                        fwrite(recvbuf, 1, size, f);
                        iResult = iResult - size;

                        // nếu ghi đủ chunk data nhưng thiếu \r\n thì cập nhật lại crlfShortage
                        if (iResult >= 2) {
                            iResult = iResult - 2;
                            memmove(recvbuf, recvbuf + size + 2, iResult);
                            memset(recvbuf + iResult, '\0', DEFAULT_BUFLEN - iResult);
                            cout << "so byte thua: " << iResult << endl;
                            cout << "data thua: " << recvbuf << endl;
                        } else {
                            crlfShortage = true;
                            data = iResult;     // lưu lại số byte còn (0 <= data <= 1 ) => CRLF thiếu (1 hoặc 2)
                            iResult = 0;
                        }
                        size = 0;
                    } else {
                        // nếu iResult < size thì ghi file số bytes lấy được và cập nhật lại size
                        fwrite(recvbuf, 1, iResult, f);
                        size -= iResult;
                        iResult = 0;
                    }
                } else { // nếu size = 0 mà chưa tìm thấy \r\n trong recvbuf để tách chunk size thì nhận thêm dữ liệu
                    cout << "nhan them du lieu de tach chunk size" << endl;
                    memmove(chunkData, recvbuf, iResult);
                    data = iResult;
                    iResult = 0;
                }
            } else { // size > 0 : tức là chunk data còn thiếu
                if (iResult >= size) {
                    cout << "ghi file chunk data: \n\n" << recvbuf << endl;
                    fwrite(recvbuf, 1, size, f);
                    iResult = iResult - size;
                    if (iResult >= 2) {
                        cout << "ghi du CRLF " << endl;
                        iResult = iResult - 2;
                        memmove(recvbuf, recvbuf + size + 2, iResult);
                        memset(recvbuf + iResult, '\0', DEFAULT_BUFLEN - iResult);
                    } else {
                        cout << "thieu CRLF" << endl;
                        crlfShortage = true;
                        data = iResult;
                        iResult = 0;
                    }
                    size = 0;
                } else {
                    fwrite(recvbuf, 1, iResult, f);
                    size -= iResult;
                    iResult = 0;
                }
            }

        } while (iResult > 0 ); 

        if (size != -1) {
            memset(recvbuf, '\0', DEFAULT_BUFLEN);
            iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

            if (iResult == SOCKET_ERROR || iResult == 0) {
                int err= WSAGetLastError();
                cout << "recv failed with error: \n" << err << endl;
                if (err == WSAEWOULDBLOCK) {  // currently no data available
                    Sleep(100);  // wait and try again: 100ms
                    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
                } else {
                    return 225;
                }
            }

        }
        cout << "Bytes can nhan them: " << size << endl;
        cout << "Byte Received: = " << iResult << endl;
    }
    cout << "line 372" << endl;
    // đóng file
    fclose(f);
    
    // giải phóng bộ nhớ
    cout << "giải phóng bộ nhớ" << endl;
    free(chunkSize);
    free(chunkData);

    cout << "line 382" << endl;
    return 225;   
}

int clientSocket::downloadFolder(char *folderName, char *path)
{
    cout << "downloading folder" << endl;

    // tạo thư mục
    if (mkdir(folderName) == -1) 
    {
        cout << "Error creating directory!\n";
        exit(1);
    }

    // tải file index.html
    char *fileName = (char *)malloc(1024);
    memset(fileName, '\0', 1024);
    strcpy(fileName, folderName);
    strcat(fileName, "\\index.html");

    downloadFile(fileName, path);

    // lọc các link trong file index.html và lưu vào vector link
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        cout << "Error opening file! \n";
        exit(1);
    }

    char *line = (char *)malloc(1024);
    memset(line, '\0', 1024);
    char ** links = (char **)malloc(1024);
    int linkCount = 0;

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
                    links[linkCount] = (char *)malloc(1024);
                    memset(links[linkCount], '\0', 1024);
                    strcpy(links[linkCount], linkName);
                    linkCount++;
                    cout << "link " << linkCount <<": " << linkName << endl;
                }
            }
        }
    }

    // multiple requests cho các requests tải các file trong folder
    multipleRequest(links, linkCount, path, folderName);

    //  đóng file
    fclose(f);
    
    return 225;
}


int clientSocket::multipleRequest(char ** links, int linkCount, char *path, char *folderName) {
        cout << "multiple request" << endl;

        // tạo request đến các trang trong links để tải file
        for (int i = 0; i < linkCount; i++) {
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
            
            // Tính thời gian tải file
            int start = clock();
            downloadFile(fileName, newPath);
            int end = clock();
            cout << "Time to download: " << 1000 * (end - start)/CLOCKS_PER_SEC << "ms" << endl;

            // nếu response header không cho keep alive thì đóng kết nối, mở kết nối mới
            if (isKeepAlive == false) {
                closesocket(ConnectSocket);
                connectToServer();
            }
        }
 
}

void clientSocket::closeConnection() {
    cout << "closing connection" << endl;

    // shutdown the connection 
    iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed with error: \n" << WSAGetLastError() << endl;
    }

    // close socket
    closesocket(ConnectSocket);
}


