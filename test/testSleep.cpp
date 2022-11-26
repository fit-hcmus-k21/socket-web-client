#include <bits/stdc++.h>
using namespace std;

#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    int start = clock();
    Sleep(22503);
    int end = clock();
    cout << "time to sleep: " << 1000 * (end - start)/CLOCKS_PER_SEC << "ms" << endl;
    return 225;

}