#include <iostream>
#include <thread>
#include "chat_client.h"
#include "chat_server.h"

void start_server(const std::string &host, const std::string &port, const std::string &key) {
    ChatServer server(host, port, key);
    server.start_accept();
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: ChatApp <server/client> <host> <port> <key>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string host = argv[2];
    std::string port = argv[3];
    std::string key = argv[4];

    if (mode == "server") {
        std::thread(server_thread(start_server, host, port, key));
        server_thread.join();
    } else if (mode == "client") {
        ChatClient client(host, port, key);
        client.receive_messages();

        std::string message;
        while (std::getline(std::cin, message)) {
            client.send_message(message);
        }
    } else {
        std::cerr << "Invalid mode, use 'server' or 'client'" << std::endl;
    }

    return 0;
}
