#include "chat_server.h"
#include "message_db.h"
#include <iostream>

ChatServer::ChatServer(const std::string &host, const std::string &port, const std::string &key)
    : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(host), std::stoi(port))),
      encryption_key(key), message_db("chat_history.db") {
    message_db.initialize();
    start_accept();
}

void ChatServer::handle_message(const std::string &message, std::shared_ptr<boost::asio::ip::tcp::socket> sender_socket) {
    std::string decrypted_message = Encryption::decrypt(message, encryption_key);
    message_db.save_message(decrypted_message); // 持久化消息
    std::cout << "Received: " << decrypted_message << std::endl;

    for (const auto &client_socket : client_sockets) {
        if (client_socket != sender_socket) {
            boost::asio::write(*client_socket, boost::asio::buffer(message)); // 广播给其他客户端
        }
    }
}

void ChatServer::fetch_persisted_messages() {
    message_db.fetch_all_messages(); // 可选调用以获取历史消息
}
