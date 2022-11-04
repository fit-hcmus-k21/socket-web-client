#include <bits/stdc++.h>
using namespace std;

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) {
    // nếu có http:// thì bỏ đi
    if (strstr(url, "http://") != NULL) {
        host = url + 7;
    } else {
        if (strstr(url, "https://") != NULL) {
            host = url + 8;
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

int main() {
    char *url = (char *)malloc(100);
    char *host = (char *)malloc(100);
    char *path = (char *)malloc(100);
    strcpy(url, "http://example.com/file");
    cout << "url: " << url << endl;

    splitPath(url, host, path);
    cout << "host: " << host << endl;
    cout << "path: " << path << endl;
    return 0;
}