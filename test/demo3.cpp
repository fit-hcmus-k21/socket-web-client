#include <bits/stdc++.h>
using namespace std;

int main() {
    // read date from alice.txt and write to bob.txt
    FILE *f1 = fopen("alice.txt", "r");
    FILE *f2 = fopen("bob.txt", "w");
    char *line = (char *)malloc(100);
    while (fgets(line, 100, f1) != NULL) {
        fputs(line, f2);
    }
    fclose(f1);
    fclose(f2);

    string s = "hello world\r\nHUhu\r\nHic\rhmm";
    cout << s << endl;
    return 0;
}