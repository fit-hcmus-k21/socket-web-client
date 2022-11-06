#include <bits/stdc++.h>
using namespace std;

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
            cout << "huhu\n";

            char *q = p;
            q--;
            while (q != path) {
                if (*q == '/') {
                    break;
                }
                q--;
            }
            cout << "q: " << q << endl;
            cout << "p: " << p << endl;
            cout << "path: " << path << endl;
            // lấy tên folder: copy từ vị trí q + 1 đến p
            strncpy(folderName, q + 1, p - (q + 1) );
            cout << "folderName: " << folderName << endl;
        }

    } else {
        // nếu path là /file
        fileName = path + 1;
    }
}

int main() {
    char *path = new char[100];
    strcpy(path, "/");
    char *fileName = new char[100];
    char *folderName = new char[100];
    memset(fileName, 0, 100);
    memset(folderName, 0, 100);
    getFileName(path, fileName, folderName);
    if (strlen(fileName) > 0) {
        cout << "fileName: " << fileName << endl;
    } else {
        cout << "folderName: " << folderName << endl;
    }

    delete [] fileName;
    delete [] folderName;
    delete [] path;

    return 0;
}