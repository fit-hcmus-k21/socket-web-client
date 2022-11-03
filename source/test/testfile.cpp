#include <bits/stdc++.h>
using namespace std;

int main() {
    FILE *f = fopen("test.txt", "wb");
    char *c = (char *)malloc(100);
    strcpy(c, "1");
    for (int i = 0 ; i < 22; i ++) { 
        char * b ;
        b = (char *)malloc(100);
        // biến số nguyên thành kí tự và gán vào b
        sprintf(b, "%d", i);
        printf("b: %s", b); 
        strcpy(c, b);
        fwrite(c,1, 1, f);
    }
    fclose(f);
    return 0;
}