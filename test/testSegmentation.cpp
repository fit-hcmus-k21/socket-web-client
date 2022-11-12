#include <bits/stdc++.h>
using namespace std;

int main() {
    char *p = (char *)malloc(0);
    p[0] = 'a' ;
    cout << p[0] << endl;
    free(p);
    cout << "freed p" << endl;
    free (p);
    cout << "freed p again" << endl;
    return 0;
}