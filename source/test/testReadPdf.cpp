#include <bits/stdc++.h>
using namespace std;

int main() {
    // read pdf file and save to output.pdf
    FILE *f = fopen("KHHT.pdf", "rb");
    FILE *f2 = fopen("output.pdf", "wb");
    char *buffer = (char *)malloc(1000000);
    int n = fread(buffer, 1, 1000000, f);
    fwrite(buffer, 1, n, f2);
    while (n > 0) {
        // memset(buffer, 0, 1000000);
        n = fread(buffer, 1, 1000000, f);
        fwrite(buffer, 1, n, f2);
    }


    fclose(f);
    fclose(f2);
    return 0;
    
}