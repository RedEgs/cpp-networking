//
// Created by Charlie on 12/01/2026.
//

#ifndef NETWORK_TESTING_SERVER_H
#define NETWORK_TESTING_SERVER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstring>
#include <vector>


#pragma comment(lib, "ws2_32.lib")


class Server {
private:
    std::vector<SOCKET> client_sockets;
    std::vector<std::string> client_names;

    static void init_wsa();
    SOCKET server_socket;

public:
    bool listening = false;


    Server(short port);
    int listen_for_connections(int max_pending_connections);
    int accept_connection();

    std::vector<char> receive_from_socket(int client_socket_index) ;
    void send_to_socket(const char *buffer, int client_socket_index) const;
    void close() const;
    void close_client_connection(int client_socket_index);

    std::string get_client_name(int client_socket_index);

};


#endif //NETWORK_TESTING_SERVER_H