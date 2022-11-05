#include <bits/stdc++.h>
using namespace std;

int main() {
    // strstr cua null
    char *a = (char *)malloc(100);
    char *b = (char *)malloc(100);
    strcpy(a, "\0");
    strcpy(b, "abc");
    if (strstr(a, b) != NULL) {
        cout << "a co chua b";
    } else {
        cout << "a khong chua b";
    }

    return 0;
}