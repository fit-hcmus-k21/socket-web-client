#include <bits/stdc++.h>
using namespace std;

int main() {
    // read presentation.ppt and save to output2.ppt
    FILE *f = fopen("presentation.pptx", "rb");
    FILE *f2 = fopen("output2.pptx", "wb");
    if (f == NULL || f2 == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    char c;
    while (fread(&c, 1, 1, f) == 1) {
        fwrite(&c, 1, 1, f2);
    }
    fclose(f);
    fclose(f2);
    return 0;
}