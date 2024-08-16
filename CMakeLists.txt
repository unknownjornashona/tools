cmake_minimum_required(VERSION 3.10)
project(ChatApp)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

add_executable(ChatApp main.cpp encryption.cpp chat_client.cpp chat_server.cpp message_db.cpp)
target_link_libraries(ChatApp Boost::system OpenSSL::SSL OpenSSL::Crypto sqlite3)
