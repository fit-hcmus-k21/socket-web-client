/* Yêu cầu: */

/*
Handle connection cơ bản:
Client tạo một TCP socket, kết nối đến port 80 của web server. Sau khi kết nối đến web server, client sẽ gửi request đến web server để tải page. Client yêu cầu sử dụng HTTP/1.1 và "Connection: keep-alive", khi đó hàm "recv" sẽ vẫn chờ nhận dữ liệu mặc dù đã nhận đủ nội dung file. Sinh viên cần tính toán để biết khi nào đã nhận xong data và đóng kết nối. Vậy làm sao Client có thể biết được là đã nhận đủ data, thì bạn dựa vào một trong các trường hợp:
"Content-Length:" cho biết nội dung của body hoặc
"Transfer-Encoding: chunked", khi đó webserver sẽ trả về dữ liệu từng chunk, làm sao nhận biết được độ dài 1 chunk và khi nào nhận hết các chunk, Tham khảo: https://en.wikipedia.org/wiki/Chunked_transfer_encoding, https://bunny.net/academy/http/what-is-chunked-encoding/
Yêu cầu tải và lưu file
File download hay folder download sẽ nằm cùng cấp với binary 
Với các request là "/" gốc thì mặc định tải và lưu thành file “<domain>_index.html”, Ví dụ
http://example.com hoặc http://example.com/, thì client sẽ tải và lưu thành file "example.com_index.html"
http://www.bing.com hoặc http://www.bing.com/, thì client sẽ tải và lưu thành file "www.bing.com_index.html"
Với các request tải một file, ví dụ *.html, *.pdf, *.jpg ..., thì client sẽ tải và lưu thành tên file tương ứng kèm theo domain theo cấu trúc: "<domain>_<tenfile>". Ví dụ: 
http://example.com/index.html, thì sẽ tải và lưu thành file “example.com_index.html”
http://web.stanford.edu/dept/its/support/techtraining/techbriefing-media/TB-080610-ProtectYourData.ppt, thì sẽ tải và lưu thành file “web.stanford.edu_TB-080610-ProtectYourData.ppt”
Với request là các subfolder, thì download hết tất cả các file trong folder đó kèm domain theo cấu trúc: "<domain>_<tenfolder>". Ví dụ: 
http://web.stanford.edu/class/cs224w/slides/ thì tải và lưu hết tất cả các file vào folder “web.stanford.edu_slides”, trong folder đó sẽ chứa các file tương ứng mà không có domain kèm theo.
Nâng cao: 
Cho phép gửi nhiều request cho mỗi kết nối: Hiện tại client mở một TCP connection cho mỗi request khi nhiều file cần được truy cập => ko hiệu quả, HTTP cho phép gửi nhiều request trên một kết nối trước đó. Nếu server không có header "Connection: close" trong response, client có thể giữ kết nối, và tiếp tục gửi những request tiếp theo nếu client gửi có header "Connection: keep-alive". Để cho phép Client có thể phân biệt data từ nhiều request, server bắt buộc phải có "Content-Length:" hoặc "Transfer-Encoding: chunked" header để chỉ định kích thước của data trả về cho client. Kết nối chỉ đóng được khi client đóng kết nối, khi đã tải xong hoàn toàn dữ liệu. Áp dụng khi download nhiều file trong subfolder. Ví dụ:
./<tên file chạy> http://web.stanford.edu/class/cs224w/slides/
Cho phép client tải nhiều trang cùng lúc (Handling Multiple Connections in Parallel, concurrency by thread for each connection, ...). Áp dụng khi Client có nhiều link download cùng lúc. Ví dụ: 
./<tên file chạy> http://web.stanford.edu/class/cs231a/project.html http://web.stanford.edu/class/cs224w/slides/08-GNN-application.pdf ...
*/

// 1. Kết nối: Cho phép client kết nối đến server thông qua kết nối TCP

// 2. Quản lý kết nối: khi client hoặc server mất kết nối đột ngột không làm chương trình treo hay xảy ra lỗi

// 3. Tải và lưu file thành dạng "content-length" : download file thành công

// 4. Tải các file trong folder : lưu tất cả các file trong folder

// 5. Multiple requests : gửi nhiều requests trong một connection khi download file trong folder

// 6. Multiple connection: concurrent, handle nhiều kết nối cùng lúc đến các web servers


#define _WIN32_WINNT 0x0501 
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 2000
#define DEFAULT_PORT "80"


int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, // result is a linked list of addrinfo structures, each of which contains an Internet address that can be specified in a call to the bind function.
                    *ptr = NULL, // ptr is a pointer to an addrinfo structure, which is used to traverse the list of addrinfo structures
                    hints; // hints is an addrinfo structure that provides gợi ý about the type of socket the caller supports
    const char *sendbuf = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";

    
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port

    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) { // ptr->ai_next is a pointer to the next addrinfo structure in the list

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol); // ptr->ai_family is the address family, ptr->ai_socktype is the socket type, ptr->ai_protocol is the protocol
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    
    // Receive until the peer closes the connection
    
	memset(recvbuf, 0, recvbuflen);
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if ( iResult > 0 ) {
		// I assume that I had already received enough data.
        printf("Bytes received: %d\n", iResult);
		printf("Data:\n%s", recvbuf);
	}
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}