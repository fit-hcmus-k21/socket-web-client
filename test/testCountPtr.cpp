#include <bits/stdc++.h>
using namespace std;

int main() {
    char *a = (char *)malloc(100);
    char *b = (char *)malloc(100);

    strcpy(a, "abc");
    strcpy(b, "def123");

    cout << "length a: " << strlen(a) << endl;
    cout << "length b: " << strlen(b) << endl;
    cout << "size of char* " << sizeof(char *) << endl;

    return 0;
}