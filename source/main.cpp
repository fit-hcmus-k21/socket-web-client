#include "library.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int __cdecl main(int argc, char **argv) {
    // validate the parameters
    if (argc < 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }
    if (argc == 2) {
        // single connection
        handleConnection(argv[1]);
        cout << "main 16" << endl;
    } else {
        // multiple connection
        handleMultipleConnection(argc - 1, argv + 1);
    }
    cout << "finished" << endl;

    return 225;
}