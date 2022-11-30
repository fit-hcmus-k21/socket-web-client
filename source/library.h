#include "clientSocket.h"

#include <pthread.h>
#include <direct.h>
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <windows.h>

#include <iostream>
using namespace std;

// tách host và path từ url
void splitPath(char *url, char *&host, char *&path) ;

// lấy fileName/ folderName từ path
void getFileName(char *path, char *&fileName, char *&folderName) ;

// tách header, body từ response
void splitResponse(char *&response, char *&header, char *&body) ;

// hàm tách các thPm số truyền vào từ command line ra vector các link
void splitLink(char *link, char **linkList, int &count) ;

// hàm chỉnh tên file/folder lưu vào directory
void createNewFName (char *fname, char *host, char *dir, char *newFName) ;

// xử lý thông tin từ input và trả về thông tin cần thiết
void processURL(char *url, char *&host, char *&path, char *&fileName, char *&folderName) ;

// handle a connection
bool handleConnection(char *url);

// handle multiple connection
void handleMultipleConnection(int n, char **urls);

// --------Các hàm hỗ trợ--------
void gotoxy(short x, short y) ;

void textcolor(int x) ;