#include <bits/stdc++.h>
using namespace std;

int main() {
    // mở file test.txt để đọc ở chế độ binary
    FILE *fin = fopen("test.txt", "rb");
    char *buf = (char *)malloc(1000000);
    memset(buf, '\0', 1000000);
    int n = fread(buf, 1, 1000000, fin);
    cout << "n = " << n << endl;
    cout << "BUFFER: " << buf << endl;

    FILE *fout = fopen("output.txt", "wb");
    fwrite(buf, 1, n, fout);

    fclose(fout);
    fclose(fin);

    return 0;
}