#include "clientSocket.h"

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) ;

// lấy fileName/ folderName từ path
void getFileName(char *path, char *&fileName, char *&folderName) ;

