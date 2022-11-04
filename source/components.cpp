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
        fileName = path + 1;
    }
}

// tách header, body, content-type, content-length từ response
void splitResponse(char *response, char *&header, char *&body, char *&contentType, char *&contentLength) {
    // tìm vị trí của \r\n\r\n
    char *p = strstr(response, "\r\n\r\n");
    if (p != NULL) {
        // tách header
        header = response;
        // tách body
        body = p + 4;
        // tìm vị trí của content-type
        char *q = strstr(header, "Content-Type: ");
        if (q != NULL) {
            // tách content-type
            contentType = q + 14;
            // tìm vị trí của \r\n
            char *r = strstr(contentType, "\r\n");
            if (r != NULL) {
                strncpy(contentType, contentType, r - contentType);
            }
            
        }
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
        }
    }
    *p = '\0';
}