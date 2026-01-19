//
// Created by Charlie on 12/01/2026.
//

#include "server.h"


#define WIN32_LEAN_AND_MEAN
#include <array>
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


#pragma comment(lib, "ws2_32.lib")


Server::Server(short port) {
    init_wsa();

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to initialise server socket during initialisation.\n";
        return;
    }
    server_socket = serverSocket;

    sockaddr_in serverAddress{
        .sin_family = AF_INET,
        .sin_port = htons(port),

    };
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind server during initialisation.\n";
        closesocket(server_socket);
        return;
    }
}

int Server::listen_for_connections(int max_pending_connections = SOMAXCONN) {
    /* Returns -1 if the listen() failed, returns 0 if listening. */
    if (listen(server_socket, max_pending_connections) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen.\n";
        listening = false;
        return -1;
    }
    listening = true;
    return 0;
}

int Server::accept_connection() {
    /* Returns -1 if it failed to accept, returns the index of socket that it accepted. */
    sockaddr_storage their_addr{};
    socklen_t their_len = sizeof(their_addr);

    SOCKET clientSocket = accept(server_socket, reinterpret_cast<struct sockaddr *>(&their_addr), &their_len);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept connection.\n";
        return -1;
    }

    sockaddr_in *cAddr = reinterpret_cast<struct sockaddr_in *>(&their_addr);
    in_addr ipAddr = cAddr->sin_addr;

    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

    client_sockets.push_back(clientSocket);
    client_names.push_back(str);

    return client_sockets.size()-1;
}

std::vector<char> Server::receive_from_socket(int client_socket_index) {
    std::vector<char> buffer(1024);

    int n = recv(client_sockets[client_socket_index], buffer.data(), buffer.size(), 0);
    if (n == 0) {

        close_client_connection(client_socket_index);
        std::cerr << "Client closed connection" << std::endl;

        return {};
    }
    if (n < 0) {
        close_client_connection(client_socket_index);
        std::cerr << "Connection to client failed" << std::endl;
        return {};
    }

    buffer.resize(n);
    return buffer;
}

void Server::send_to_socket(const char* buffer, int client_socket_index) const {
    int sent = 0;
    int len = strlen(buffer);
    while (sent < len) {
        int n = send(client_sockets[client_socket_index], buffer + sent, len - sent, 0);
        if (n <= 0) return;
        sent += n;
    }
}


    void Server::init_wsa() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed\n";
            std::exit(1);
        }
    }


void Server::close() const {
    closesocket(server_socket);
}

void Server::close_client_connection(int client_socket_index) {
    closesocket(client_sockets[client_socket_index]);
    client_sockets.erase(client_sockets.begin() + client_socket_index);
    client_names.erase(client_names.begin() + client_socket_index);
}

std::string Server::get_client_name(int client_socket_index) {
    return client_names[client_socket_index].c_str();
}

