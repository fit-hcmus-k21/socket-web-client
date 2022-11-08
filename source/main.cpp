#include "library.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")


int __cdecl main(int argc, char **argv) {
    // validate the parameters
    if (argc < 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    } else {
    }

    // Tách host và path từ url
    char *url = (char *)malloc(100);
    char *path = (char *)malloc(100);
    char *host = (char *)malloc(100);
    memset(path, '\0', 100);
    memset(host, '\0', 100);
    strcpy(url, argv[1]);
    splitPath(url, host, path);

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

    char *dir = (char *)malloc(100);
    strcpy(dir, "releases\\");

    // nếu là file 
    if (strlen(fileName) > 0) {
        // chỉnh tên file theo cấu trúc: host_fileName, lưu vào folder releases
        char *newFileName = createNewFName(fileName, host, dir);

        client.downloadFile(newFileName);
    } else {
        // nếu là folder: chỉnh tên folder theo cấu trúc: host_folderName, lưu vào folder releases
        char *newFolderName = createNewFName(folderName, host, dir);

        client.downloadFolder(newFolderName);
    }

    client.closeConnection();

    return 225;

}