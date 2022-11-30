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

    // thiết kế giao diện chương trình

    return 225;
}