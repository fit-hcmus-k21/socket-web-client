#include "clientSocket.h"

#include <pthread.h>
#include <direct.h>
#include <vector>

#include <iostream>
using namespace std;

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) ;

// lấy fileName/ folderName từ path
void getFileName(char *path, char *&fileName, char *&folderName) ;

// tách header, body từ response
void splitResponse(char *&response, char *&header, char *&body) ;

// hàm tách các thPm số truyền vào từ command line ra vector các link
void splitLink(char *link, vector<char *> &linkList) ;

// hàm chỉnh tên file/folder lưu vào directory
void createNewFName (char *fname, char *host, char *dir, char *newFName) ;

// xử lý thông tin từ input và trả về thông tin cần thiết
void processURL(char *url, char *&host, char *&path, char *&fileName, char *&folderName) ;

// handle a connection
void *handleConnection(char *url);

// handle multiple connection
void *handleMultipleConnection(int n, char **urls);