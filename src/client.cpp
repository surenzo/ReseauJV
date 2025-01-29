
#include <iostream>
#include <Socket.h>

int main(int argc, char **argv)
{
    try {
        Socket clientSocket("127.0.0.1","5555");
        char messageSend[512];
        scanf("%511s", messageSend);
        clientSocket.sendToServer(messageSend);

        char message[512];
        clientSocket.receiveFromServer(message);
        printf("Message received: %s\n", message);

    }catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
