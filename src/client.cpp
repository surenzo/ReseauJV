
#include <iostream>
#include <Socket.h>

int main(int argc, char **argv)
{
    try {
        Socket clientSocket("127.0.0.1","5555");
        clientSocket.sendToServer("Hello world");

        char message[512];
        clientSocket.receiveFromServer(message);

    }catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
