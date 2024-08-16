#include "chat_server.h"
#include <iostream>

ChatServer::ChatServer(const std::string &host, const std::string &port, const std::string &key)
    : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host), std::stoi(port))),
      encryption_key(key), message_db("chat_history.db") {
    message_db.initialize();
    start_accept();
}


void ChatServer::start_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    acceptor.async_accept([this, socket](const boost::system::error_code &error) {
        if (!error) {
            client_sockets.insert(socket);
            handle_accept(socket);
        }
        start_accept();
    });
}

void ChatServer::handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    std::thread([this, socket]() {
        try {
            while (true) {
                char buffer[1024];
                boost::system::error_code error;

                std::size_t len = socket->read_some(boost::asio::buffer(buffer), error);

                if (error) {
                    client_sockets.erase(socket);
                    break;
                }

                std::string encrypted_message(buffer, len);
                handle_message(encrypted_message, socket);
            }
        } catch (...) {
            client_sockets.erase(socket);
        }
    }).detach();
}

void ChatServer::handle_message(const std::string &message, std::shared_ptr<boost::asio::ip::tcp::socket> sender_socket) {
    std::string decrypted_message = Encryption::decrypt(message, encryption_key);
    std::cout << "Received: " << decrypted_message << std::endl;

    for (const auto &client_socket : client_sockets) {
        if (client_socket != sender_socket) {
            boost::asio::write(*client_socket, boost::asio::buffer(message)); // 广播给其他客户端
        }
    }
}
