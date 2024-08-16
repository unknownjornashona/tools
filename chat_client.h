#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <boost/asio.hpp>
#include <string>
#include "encryption.h"

class ChatClient {
public:
    ChatClient(const std::string &host, const std::string &port, const std::string &key);
    void connect();
    void send_message(const std::string &message);
    void receive_messages();

private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket;
    std::string encryption_key;
};

#endif // CHAT_CLIENT_H
