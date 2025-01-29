
#include <iostream>
#include <Socket.h>


int  main() {
    try {
        Socket serverSocket("5555");

        char message[512];
        serverSocket.listen(message);

    }catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;

}
