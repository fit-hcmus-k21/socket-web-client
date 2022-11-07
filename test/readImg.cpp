// read demoImg and save to ouputImg.jpg

#include <bits/stdc++.h>
using namespace std;

int main() {
    FILE *fin = fopen("demoImg.jpg", "rb");
    FILE *fout = fopen("outputImg.jpg", "wb");

    char *buf = (char *)malloc(1000000);
    memset(buf, '\0', 1000000);
    int n = fread(buf, 1, 1000000, fin);
    cout << n << endl;
    cout << strlen(buf) << endl;
    cout << "dat:\n\n" << buf << endl;
    fwrite(buf, 1, 1000000, fout);

    cout << "data: \n\n" ;
    for (int i = 0; i < 100; i++) {
        cout << buf[i] << " ";
    }

    fclose(fin);
    fclose(fout);

    return 0;
}