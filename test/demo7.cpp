// test create folder and download file save to that folder

#include <bits/stdc++.h>
#include <direct.h>
using namespace std;

int main() {
    // create folder
    char *folderName = (char *)malloc(100);
    memset(folderName, '\0', 100);
    strcat(folderName, "releases\\");

    // tạo folder
    _mkdir(folderName);

    strcat(folderName, "hic.txt");
    
    // ghi vao file
    FILE *fp = fopen(folderName, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(fp, "This is testing for fprintf...\n");
    fputs("This is testing for fputs...\n", fp);
    fclose(fp);
    return 225;
}