<!-- tổ chức source code -->

```tsx
                                            TỔ CHỨC SOURCE CODE

                                        ___________release 
                                        |             |__________a.exe
                                        |
                                        |___________source
                                        |             |
                                        |             |___________clientSocket.cpp
                                        |             |
                                        |             |___________clientSocket.h
                                        |             |
                                        |             |___________library.h
                                        |             |
                                        |             |___________components.cpp
                                        |             |
                                        |             |___________main.cpp
                                        |
                                        |___________report.pdf


Trong đó: 
`release`: chứa file thực thi của chương trình
`source`: 
        - `clientSocket.cpp`: chứa các hàm xử lý trên socket, định nghĩa các hàm trong `clientSocket.h`
        - `clientSocket.h`: chứa các khai báo hàm được tổ chức dưới dạng class
        - `library.h`: chứa các khai báo hàm hỗ trợ xử lý chuỗi, tách header, ghép tên file, thư mục, ...
        - `components.cpp`: chứa các định nghĩa hàm được khai báo trong `library.h`
        - `main.cpp`: thiết kế giao diện chương trình, xử lý các tham số đầu vào, gọi các hàm trong `clientSocket.cpp`
`report.pdf`: báo cáo đồ án, mô tả các chức năng, cấu trúc chương trình, ...

```

