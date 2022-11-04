#include "library.h"

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) {
    // nếu có http:// thì bỏ đi
    if (strstr(host, "http://") != NULL) {
        host = host + 7;
    } else {
        if (strstr(host, "https://") != NULL) {
            host = host + 8;
        }
    }
    // tìm vị trí của dấu /
    char *p = strchr(url, '/');
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