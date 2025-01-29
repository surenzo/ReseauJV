//
// Created by super on 28/01/2025.
//

#ifndef SOCKET_H
#define SOCKET_H

class Socket {
    public:
        Socket(const char* ip, const char* port);
        ~Socket();
        Socket(const char*port); // server version
        void sendToServer(const char* message);
        void receiveFromServer(char* message);
        void listen(char* message);
};

#endif //SOCKET_H
