#include "library.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")


int __cdecl main(int argc, char **argv) {
    // validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // nếu có http:// thì bỏ đi
    char *host = argv[1];
    if (strstr(host, "http://") != NULL) {
        host = host + 7;
    } else {
        if (strstr(host, "https://") != NULL) {
            host = host + 8;
        }
    }

    // tách host và filename/ folder từ đường dẫn
    char *path = (char *)malloc(100);

    char *p = strchr(host, '/'); // tìm vị trí xuất hiện ký tự '/'
    if (p != NULL) {
        // nếu host la example.com/
        if (strlen(p) == 1) {
            strcpy(path, "/");
        } else {
            // nếu host la example.com/file 
            path = p + 1;
            // chèn / vào đầu path
            char *temp = (char *)malloc(100);
            strcpy(temp, "/");
            strcat(temp, path);
            path = temp;
        }
        *p = '\0';
    } else {
         strcpy(path, "/");
    }

    // tách file hoặc folder từ path
    char *fileName = (char *)malloc(100);
    memset(fileName, '\0', 100);
    char *folderName = (char *)malloc(100);
    memset(folderName, '\0', 100);

    // lấy tên fileName hoặc folderName từ path
    getFileName(path, fileName, folderName);

    clientSocket client = clientSocket();
    client.init();
    client.connectToServer(host);

    char *request = (char *)malloc(100);
    request = client.createRequest(host, path);
    client.sendRequest(request);

    printf("Host: %s\n", host);
    printf("File: %s\n", fileName);
    printf("request: %s\n", request);

    // nếu là file
    if (strchr(fileName, '.') != NULL) {
        client.downloadFile(host, fileName);
        // client.downloadFileChunk(host, fileName);
    } else { // nếu là folder
        // client.downloadFolder(host, folder);
    }

    client.closeConnection();

    return 225;

}