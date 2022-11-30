#include "library.h"

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) {
    // nếu có http:// thì bỏ đi
    if (strstr(url, "http://") != NULL) {
        url += 7;
    } else {
        if (strstr(url, "https://") != NULL) {
            url += 8;
        }
    }
    // tìm vị trí của dấu /
    char *p = strchr(url, '/');
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
            strcpy(path, temp);
        }
        *p = '\0';
    } else {
         strcpy(path, "/");
    }
    strcpy(host, url);
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
            strcpy(fileName, p + 1);
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
void splitResponse(char *&response, char *&header, char *&body) {
    // tìm vị trí của \r\n\r\n
    char *p = strstr(response, "\r\n\r\n");
    if (p != NULL) {
        // tách header
        int len = p - response;
        header = (char *)malloc(len + 1); 
        memset(header, '\0', len + 1);
        memmove(header, response, len);

        // tách body
        body = p + 4;
        *p = '\0';
    } 
}

// hàm tách các tham số truyền vào từ command line ra vector các link
void splitLink(char *link, char ** linkList, int &count) {
    char *p = strtok(link, " ");
    while (p != NULL) {
        *linkList = new char[strlen(p) + 1];
        strcpy(*linkList, p);
        count ++;
        linkList ++;
        p = strtok(NULL, " ");
    }
}

// hàm chỉnh tên file/folder lưu vào directory
void createNewFName (char *fname, char *host, char *dir, char *newFName) {
    memset(newFName, '\0', 100);
    strcat(newFName, dir);
    strcat(newFName, host);
    strcat(newFName, "_");
    strcat(newFName, fname);
}

// xử lý thông tin từ input và trả về thông tin cần thiết
void processURL(char *url, char *&host, char *&path, char *&fileName, char *&folderName) {
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
bool handleConnection(char *url){
    // khai báo các biến
    char *path = (char *)malloc(100);
    char *host = (char *)malloc(100);
    char *fileName = (char *)malloc(100);
    char *folderName = (char *)malloc(100);
    char *dir = (char *)malloc(100);
    strcpy(dir, "releases\\");

    // tạo thư mục releases chứa các file/folder lưu trữ
    mkdir(dir);

    // xử lý url
    processURL(url, host, path, fileName, folderName);

    bool success = 1;

    clientSocket client = clientSocket();
    client.getServerName(host);
    if (!client.connectToServer()) {
        success = 0;
    } else {
        if (!client.handleRequest( path, fileName, folderName, dir)) success = 0;
    }

    // giải phóng bộ nhớ
    free(path);
    free(host);
    free(fileName);
    free(folderName);
    free(dir);

    if (success) {
        return 1;
    } else {
        return 0;
    }
}

// handle multiple connection
void handleMultipleConnection(int n, char **urls) {
    // khai báo các biến
    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));

    // cout << "start handle multiple connection" << endl;
    
    // tạo nhiều thread để xử lý nhiều url
    for (int i = 0; i < n; i++) {
        // cout << "creating thread " << i + 1 << endl;
        pthread_create(&threads[i], NULL, (void *(*)(void *))handleConnection, urls[i]);
    }

    // join các thread
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // cout << "handle multiple connection done" << endl;

    // giải phóng bộ nhớ
    free(threads);

    
}


// ---------------------------Các hàm hỗ trợ thiết kế giao diện chương trình----------------
void gotoxy(short x,short y) {
        HANDLE hConsoleOutput;
        COORD Cursor_an_Pos = { x,y};
        hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(hConsoleOutput , Cursor_an_Pos);
} 

void textcolor(int x) {
        HANDLE mau;
        mau=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(mau,x);
}