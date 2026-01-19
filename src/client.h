//
// Created by Charlie on 12/01/2026.
//

#ifndef NETWORK_TESTING_CLIENT_H
#define NETWORK_TESTING_CLIENT_H
#include <complex.h>
#include <vector>
#include <winsock2.h>


class Client {
public:
    bool connected = false;

    Client(const char* address, short port);

    int connect_to_server();
    void send_to_server(const char *buffer) const;

    std::vector<char> receive_from_server();

    void close();

private:
    static void init_wsa();
    SOCKET client_socket;
    sockaddr_in server_addr;
};


#endif //NETWORK_TESTING_CLIENT_H