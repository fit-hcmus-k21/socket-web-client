#include <bits/stdc++.h>
using namespace std;

int main() {
    char *request = (char*) malloc(1024);
    strcpy(request, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\nheeh");
    char *p = strstr(request, "\r\n\r\n");
    cout << p << endl;
    return 0;
}