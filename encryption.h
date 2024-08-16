#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <openssl/evp.h>

class Encryption {
public:
    static std::string encrypt(const std::string &plainText, const std::string &key);
    static std::string decrypt(const std::string &cipherText, const std::string &key);
private:
    static const int AES_KEY_LENGTH = 256;
    static const int AES_BLOCK_SIZE = 16; // AES 块大小
};

#endif // ENCRYPTION_H
