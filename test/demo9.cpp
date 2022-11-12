#include <bits/stdc++.h>
using namespace std;

int main() {
    char *url = "http://example.com/path";
    char *t = strstr(url, "//");
    cout << t << endl;
    t += 2;
    cout << t << endl;
    return 225;
}