#include <bits/stdc++.h>
using namespace std;

int main () {
    // read image and save to file imageCopy.jpg
    FILE *f = fopen("image.jpg", "rb");
    FILE *f2 = fopen("imageCopy.jpg", "wb");
    
    // đọc file và ghi vào bộ nhớ đệm
    char *buffer = (char *)malloc(110000);
    // get size of file
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    cout << "size of file: " << size << endl;
    int n = fread(buffer, 1, size, f); 
    fwrite(buffer, 1, n, f2);

    fclose(f);
    fclose(f2);
    return 0;

}