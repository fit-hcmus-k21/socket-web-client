#include <bits/stdc++.h>
using namespace std;

int main() {
    char *a = new char[100];
    memmove(a, "abcd\0hihih\r\nhahahahakdnk\02656", 10);
    for (int i = 0; i < 100; i++) {
        cout << a[i] << " ";
    }
   
    return 0;
}