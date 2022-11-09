#include <bits/stdc++.h>
using namespace std;

int main() {
    char *url = (char *)malloc(100);
    char *path = (char *)malloc(100);

    char *temp = (char *)malloc(100);
    strcpy(temp, "http://example.com/path");

    url = temp;
    memmove(path, url , strlen(url));

    cout << path << endl;
    cout << "url: " << url << endl;

    memset(url, 0, strlen(url));

    cout << "path: " << path << endl;
    cout << "temp: " << temp << endl;


    return 225;
}