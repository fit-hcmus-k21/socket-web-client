// test strncpy func

#include <bits/stdc++.h>
using namespace std;

int main() {
    char *a = (char* ) malloc(100);
    strcpy(a, "Toi La Bui Thi Thanh Ngan kkk");
    char *b = (char *) malloc(100);
    strncpy(b, a + 7, 18);
    cout << b;
    return 0;
}