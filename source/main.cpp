#include "library.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int __cdecl main(int argc, char **argv) {

    // validate the parameters
    if (argc == 2) {
        // single connection
        handleConnection(argv[1]);
    } else if (argc > 2) {
        // multiple connection
        handleMultipleConnection(argc - 1, argv + 1);
    }

    //-----------------------------------  thiết kế giao diện chương trình --------------------------------------

    SetConsoleOutputCP(65001);

    // xóa màn hình
    system("cls");

    // lấy độ dài và độ rộng của màn hình
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // in ra tiêu đề chương trình
    // set màu
    textcolor(11);
    char *title = new char[100];
    strcpy(title,     "CHƯƠNG TRÌNH TẢI FILE TỪ SERVER");
    char *heading1 = new char[100];
    strcpy(heading1, "WEB CLIENT");

    // in ra - căn giữa
    gotoxy(0, 1);
    int i = 0;
    for (; i < (columns - 32) / 2 ; i++) {
        cout << "-";
    }
    cout << title;
    i += strlen(title);
    while (i < columns + 7) {
        cout << "-";
        i++;
    }
    cout << endl;
    for (i = 0; i < (columns - 10) / 2; i++) {
        cout << "-";
    }
    cout << heading1;
    i += strlen(heading1);
    while (i < columns) {
        cout << "-";
        i++;
    }

    gotoxy(0, 3);

    bool contn = 1;

    // in ra các lựa chọn
        // set màu xanh
        textcolor(10);
        cout <<"\n----------LỰA CHỌN----------" << endl;

        cout << "1. Tải file từ server" << endl;
        cout << "2. Thoát chương trình" << endl;
        textcolor(4);
        cout << "Lưu ý: ";
        textcolor(5);
        cout << "các file tải về sẽ được lưu trữ trong thư mục ";
        textcolor(3);
        cout <<"'releases' .\n";
        textcolor(10);
        cout << "-----------------------------" << endl ;

        char *choice = new char[100];  // tránh lỗi khi nhập vào chuỗi dài

    do {
        
        // in ra dòng nhập lựa chọn
        // set màu vàng
        textcolor(14);
        cout << "\nNhập lựa chọn của bạn: " ;
        // lấy lựa chọn
        textcolor(9);
        fflush(stdin);
        cin.getline(choice, 100);
        while (choice[0] != '1' && choice[0] != '2') {
            // set màu đỏ
            textcolor(12);
            cout << "Lựa chọn không hợp lệ. Vui lòng nhập lại!" << endl;
            textcolor(14);
            cout << "Nhập lựa chọn của bạn: " ;
            textcolor(9);
            cin.getline(choice, 100);
        }
        int ch = choice[0] - '0';
        // xử lý lựa chọn
        switch (ch) {
            case 1: {
                // tải file từ server
                // in ra dòng nhập link
                // set màu vàng
                textcolor(14);
                cout << "Nhập url(s) của file muốn tải: " ;
                textcolor(9);
                // lấy links
                char *inputs = new char[1000];
                fflush(stdin);
                cin.getline(inputs, 1000);

                textcolor(11);
                cout << "\nĐang xử lý, vui lòng đợi..." << endl;
                textcolor(12);

                // tách các link ra
                char ** linkList = new char*[100];
                int count = 0;
                splitLink(inputs, linkList, count);

                // xử lý
                if (count == 1) {
                    // single connection
                    if (!handleConnection(linkList[0])) {
                       // set màu đỏ
                        textcolor(12);
                        cout << "\nCó vài lỗi xảy ra ...." << endl;
                    }
                } else if (count >= 2){
                    // multiple connection
                        handleMultipleConnection(count, linkList);
                } else {
                    // set màu đỏ
                    textcolor(12);
                    cout << "\nKhông có url nào được nhập vào ?!" << endl;
                }
                // giải phóng bộ nhớ
                delete[] inputs;
                for (int i = 0; i < count; i++) {
                    delete[] linkList[i];
                }
                delete[] linkList;
            
                break;
            }
            case 2: {
                // thoát chương trình
                contn = 0;
                break;
            }
        }

    } while (contn);

    // thoát chương trình: di chuyển con trỏ xuống dưới cùng màn hình, giữa màn hình
    textcolor(11);
    char *end = new char[100];
    strcpy(end, "CHƯƠNG TRÌNH KẾT THÚC");
    gotoxy(0, rows - 2);
    for (i = 0; i < (columns - 22) / 2; i++) {
        cout << "-";
    }
    cout << end;
    i += strlen(end);
    while (i < columns + 6) {
        cout << "-";
        i++;
    }
    textcolor(15);

    // giải phóng bộ nhớ
    delete[] title;
    delete[] heading1;
    delete[] end;
    delete[] choice;

    return 225;
}