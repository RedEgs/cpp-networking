//
// Created by Charlie on 12/01/2026.
//

#include "client.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>


#pragma comment(lib, "ws2_32.lib")


Client::Client(const char* address, short port) {
    init_wsa();

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to initialise client socket during initialisation\n";
        return;
    }
    client_socket = clientSocket;

    sockaddr_in serverAddress{
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    server_addr = serverAddress;

    inet_pton(AF_INET, address, &server_addr.sin_addr);

}

int Client::connect_to_server() {
    if (connect(client_socket, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server during initialisation\n";
        return -1;
    }
    connected = true;
    return 1;
}

void Client::send_to_server(const char* buffer) const {
    int total = 0;
    int len = static_cast<int>(strlen(buffer));

    while (total < len) {
        int sent = send(client_socket, buffer + total, len - total, 0);
        if (sent <= 0) {
            // disconnected or error
            return;
        }
        total += sent;
    }
}


std::vector<char> Client::receive_from_server() {
    std::vector<char> buffer(1024);

    int n = recv(client_socket, buffer.data(), buffer.size(), 0);
    if (n == 0) {

        close();
        std::cerr << "Server closed connection" << std::endl;

        return {};
    }
    if (n < 0) {
        close();
        std::cerr << "Server to client failed or closed" << std::endl;
        return {};
    }

    buffer.resize(n);
    return buffer;
}

void Client::close() {
    closesocket(client_socket);
    connected = false;
}

void Client::init_wsa() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        std::exit(1);
    }
}
