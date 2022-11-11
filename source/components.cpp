#include "library.h"

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) {
    // nếu có http:// thì bỏ đi
    if (strstr(url, "http://") != NULL) {
        host = url + 7;
    } else {
        if (strstr(url, "https://") != NULL) {
            host = url + 8;
        } else {
            host = url;
        }
    }
    // tìm vị trí của dấu /
    char *p = strchr(host, '/');
    if (p != NULL) {
        // nếu host la example.com/
        if (strlen(p) == 1) {
            strcpy(path, "/");
        } else {
            // nếu host la example.com/path
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
}

// lấy fileName/ folderName từ path
void getFileName(char *path, char *&fileName, char *&folderName) {
    
    //  nếu path là /
    if (strcmp(path, "/") == 0) {
        strcpy(fileName, "index.html");
        return ;
    }

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
            q--;
            while (q != path) {
                if (*q == '/') {
                    break;
                }
                q--;
            }
            // lấy tên folder: copy từ vị trí q + 1 đến p
            strncpy(folderName, q + 1, p - q - 1);
        }

    } else {
        // nếu path là /file
        strcpy(fileName, path + 1);
        if (strstr(fileName, ".") == NULL) {
            // nếu file không có đuôi
            strcat(fileName, ".html");
        }
    }
}

// tách header, body từ response
void splitResponse(char *response, char *&header, char *&body) {
    // tìm vị trí của \r\n\r\n
    char *p = strstr(response, "\r\n\r\n");
    if (p != NULL) {
        // tách header
        strncpy(header, response, p - response);
        // tách body
        body = p + 4;
        *p = '\0';
    } else {
        printf("Khong co header");
        exit(1);
    }
}

// hàm tách các tham số truyền vào từ command line ra vector các link
void splitLink(char *link, vector<char *> &linkList) {
    char *p = strtok(link, " ");
    while (p != NULL) {
        linkList.push_back(p);
        p = strtok(NULL, " ");
    }
}

// hàm chỉnh tên file/folder lưu vào directory
char* createNewFName (char *fname, char *host, char *dir) {
    char *newFName = (char *)malloc(100);
    memset(newFName, '\0', 100);
    strcat(newFName, dir);
    strcat(newFName, host);
    strcat(newFName, "_");
    strcat(newFName, fname);
    return newFName;
}

// xử lý thông tin từ input và trả về thông tin cần thiết
void processInput(char *url, char *&host, char *&path, char *&fileName, char *&folderName) {
    // memset các biến
    memset(path, '\0', 100);
    memset(host, '\0', 100);
    memset(fileName, '\0', 100);
    memset(folderName, '\0', 100);

    // tách host và path từ url
    splitPath(url, host, path);

    // tách fileName và folderName từ path
    getFileName(path, fileName, folderName);

}

// handle a connection
void *handleConnection(char *url){
    // khai báo các biến
    char *path = (char *)malloc(100);
    char *host = (char *)malloc(100);
    char *fileName = (char *)malloc(100);
    char *folderName = (char *)malloc(100);
    char *dir = (char *)malloc(100);
    strcpy(dir, "releases\\");

    // xử lý url
    processInput(url, host, path, fileName, folderName);

    clientSocket client = clientSocket();
    client.connectToServer(host);
    client.handleRequest(host, path, fileName, folderName, dir);

    // giải phóng bộ nhớ
    free(path);
    free(host);
    free(fileName);
    free(folderName);
    free(dir);

    return NULL;
}

// handle multiple connection
void *handleMultipleConnection(int n, char **urls) {
    // khai báo các biến
    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));

    // tạo nhiều thread để xử lý nhiều url
    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, (void *(*)(void *))handleConnection, urls[i]);
    }

    // join các thread
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // giải phóng bộ nhớ
    free(threads);

    return NULL;
    
}