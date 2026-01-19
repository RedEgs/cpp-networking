#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <cstring>
#include <thread>

#include "src/client.h"
#include "src/server.h"

#pragma comment(lib, "ws2_32.lib")

void server() {
    Server main(8080);
    std::cout << "Started server" << std::endl;
    if (main.listen_for_connections(10) == -1) {
        std::cerr << "Failed to listen" << std::endl;
    }
    std::cout << "Listening..." << std::endl;
    while (main.listening == true) {
        int client_index = main.accept_connection();
        if (client_index == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            main.close();
        }
        std::cout << "Accepted!" << std::endl;
        std::cout << "Connected - " << main.get_client_name(client_index) << std::endl;
        main.send_to_socket("yoko", client_index);

        while (1) {
            auto data = main.receive_from_socket(client_index);
            if (data.empty()) break;
            if (static_cast<std::string>(data.data()) == "close") main.close_client_connection(client_index);

            std::cout << "From Client " << client_index << data.data() << std::endl;
            main.send_to_socket("yoko", client_index);
        }

    }
}

void client() {
    Client main("192.168.191.211", 8080);
    if (main.connect_to_server() == -1) {
        std::cerr << "Can't connect to server";
    }


    while (!main.receive_from_server().empty()) {
        std::cout << "Send to server: ";
        std::string message;
        std::cin >> message;

        if (message == "close") {
            main.close();
        }

        main.send_to_server(message.c_str());
    }

}
int main(int argc, char* argv[])
{
    bool run_server = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--server") == 0)
        {
            run_server = true;
            break;
        }
    }

    if (run_server)
    {
        server();
    }
    else
    {
        client();
    }

    WSACleanup();
    return 0;
}
