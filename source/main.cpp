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
    char *folder = (char *)malloc(100);

    // nếu path là / 
    if (strcmp(path, "/") == 0) {
        strcpy(fileName, "index.html");
    } else {
        // tìm từ vị trí sau / đầu tiên
        char *p = strchr(path + 1, '/');
        if (p != NULL) {
            // tìm đến dấu / cuối cùng trong chuỗi
            while (strchr(p + 1, '/') != NULL) {
                p = strchr(p + 1, '/');
            }
            // nếu path là /folder/file
            if (strlen(p) > 1) {
                // tách file
                fileName = p + 1;
            } else {
                // nếu path là /folder/: lấy folder là tên folder
                // tìm từ vị trí p trở về trước cho đên khi tìm thấy dấu /
                char *q = p;
                while (q != path) {
                    if (*q == '/') {
                        break;
                    }
                    q--;
                }
                // lấy tên folder: copy từ vị trí q + 1 đến p
                strncpy(folder, q + 1, p - q - 1);
            }

        } else {
            // nếu path là /file
            fileName = path + 1;
        }
    }

    // p = strrchr(path, '/'); // tìm vị trí xuất hiện ký tự '/' cuối cùng
    

    // char *path = (char *) malloc(strlen(fileName) + 1);
    // path = strcpy(path, "/");
    // path = strcat(path, fileName);

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
    } else { // nếu là folder
        // client.downloadFolder(host, folder);
    }

    client.closeConnection();

    return 225;

}