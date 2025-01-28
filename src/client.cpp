
#include "Socket.h"
#include <iostream>

int main() {
    try {
        Socket client("127.0.0.1", "5555");
        client.sendToServer("Hello Server");

        char response[DEFAULT_BUFLEN];
        client.receiveFromServer(response);

        std::cout << "Server replied: " << response << "\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
