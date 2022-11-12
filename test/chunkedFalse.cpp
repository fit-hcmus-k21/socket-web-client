
    while (true) {
        // lấy chunk size
        cout << "265" << endl;
        while (strstr(chunkSize, "\r\n") == NULL) {
            cout << "267" << endl;
            iResult = recv(ConnectSocket, recvbuf,1, 0);
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                Sleep(100);
                iResult = recv(ConnectSocket, recvbuf, 1, 0);
            } else {
                strncat(chunkSize, recvbuf, 1);
                cout << "chunkSize 274: " << chunkSize << endl;
            }
        }
        cout << "hi 274 " << endl;  
        // tách bỏ \r\n
        char *s = strstr(chunkSize, "\r\n");
        memcpy(chunkSize, chunkSize, s - chunkSize);

        cout << "chunk size 278: " << chunkSize << endl;

        // kiểm tra chunk size có chunk extension không
        s = strstr(chunkSize, ";");
        if (s != NULL) {
            strncpy(chunkSize, chunkSize, s - chunkSize);
        }

        // chuyển chunk size từ hex sang int
        size = strtol(chunkSize, NULL, 16);
        cout << "Chunk size: " << chunkSize << endl;
        cout << "size: " << size << endl;

        // nếu chunk size = 0 thì kết thúc
        if (size == 0) {
            break;
        }

        // lấy chunk data
        while (size > 0) {
            if (size > DEFAULT_BUFLEN) {
                iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    Sleep(100);
                    iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
                } else {
                    fwrite(recvbuf, 1, iResult, f);
                    size -= iResult;
                }
            } else {
                iResult = recv(ConnectSocket, recvbuf, size, 0);
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    Sleep(100);
                    iResult = recv(ConnectSocket, recvbuf, size, 0);
                } else {
                    fwrite(recvbuf, 1, iResult, f);
                    size -= iResult;
                }
            }
            cout << "iResult 315: " << iResult << endl;
            memset(recvbuf, '\0', DEFAULT_BUFLEN);
        }

        // lấy \r\n
        iResult = recv(ConnectSocket, recvbuf, 2, 0);
        if (iResult != 2) {
            cout << "Error 322" << endl;
        }
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            Sleep(100);
            iResult = recv(ConnectSocket, recvbuf, 2, 0);
        }

        // xóa dữ liệu trong buffer và chunk size
        memset(chunkSize, '\0', 1024);
        memset(recvbuf, '\0', DEFAULT_BUFLEN);

    }