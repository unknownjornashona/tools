#include "chat_client.h"
#include <iostream>

ChatClient::ChatClient(const std::string &host, const std::string &port, const std::string &key)
    : socket(io_context), encryption_key(key) {
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve(host, port));
}

void ChatClient::send_message(const std::string &message) {
    std::string encrypted_message = Encryption::encrypt(message, encryption_key);
    boost::asio::write(socket, boost::asio::buffer(encrypted_message));
}

void ChatClient::receive_messages() {
    while (true) {
        char buffer[1024];
        boost::system::error_code error;
        std::size_t len = socket.read_some(boost::asio::buffer(buffer), error);

        if (error) {
            std::cerr << "Error: " << error.message() << std::endl;
            break;
        }

        std::string encrypted_message(buffer, len);
        std::string decrypted_message = Encryption::decrypt(encrypted_message, encryption_key);
        std::cout << "Received: " << decrypted_message << std::endl;
    }
}
