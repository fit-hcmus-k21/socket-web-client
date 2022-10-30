#include <bits/stdc++.h>
using namespace std;

int main() {
    char *header = (char *)malloc(100);
    strcpy(header,"HTTP/1.1\r 200 OK\r\n Content-Type: text/html\r\n Content-Length: 100\r\n\r\n heeheh");
    char *body = (char *)malloc(1024);
    char s[] = "\r\n\r\n";
    // strstr s
    char *p = strstr(header, s);
    // lấy vào body chuỗi từ đầu đến vị trí p + 4
    body = strncpy(body, header, p - header + 4);
    printf("header: %s\n", body);
    
    return 0;

}