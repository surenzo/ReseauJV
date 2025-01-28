#include "Socket.h"
#include <iostream>

int main() {
    try {
        Socket server("5555");

        char message[DEFAULT_BUFLEN];
        while (true) {
            server.listen(message);
            std::cout << "Client said: " << message << "\n";
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
