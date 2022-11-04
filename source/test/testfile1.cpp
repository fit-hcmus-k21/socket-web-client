#include <bits/stdc++.h>
using namespace std;

int main() {
    FILE *f = fopen("outputText.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    char *str = (char*) malloc(100);
    strcpy(str, "This is testing for fprintf...\nhehe");
    fprintf(f, "%s", str);
    fclose(f);

    return 225;
}