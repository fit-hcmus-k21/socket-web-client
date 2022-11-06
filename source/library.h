#include "clientSocket.h"
#include <bits/stdc++.h>
using namespace std;

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) ;

// lấy fileName/ folderName từ path
void getFileName(char *path, char *&fileName, char *&folderName) ;

// tách header, body từ response
void splitResponse(char *response, char *&header, char *&body) ;