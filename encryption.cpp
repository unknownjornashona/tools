#include "encryption.h"
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <cstring>

std::string Encryption::encrypt(const std::string &plainText, const std::string &key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> cipherText(plainText.size() + AES_BLOCK_SIZE);
    int len = 0;

    // 初始化加密操作
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char*)key.c_str(), nullptr);
    EVP_EncryptUpdate(ctx, cipherText.data(), &len, (const unsigned char*)plainText.c_str(), plainText.size());
    int cipherTextLen = len;

    EVP_EncryptFinal_ex(ctx, cipherText.data() + len, &len);
    cipherTextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(cipherText.begin(), cipherText.begin() + cipherTextLen);
}

std::string Encryption::decrypt(const std::string &cipherText, const std::string &key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> plainText(cipherText.size());
    int len = 0;

    // 初始化解密操作
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (const unsigned char*)key.c_str(), nullptr);
    EVP_DecryptUpdate(ctx, plainText.data(), &len, (const unsigned char*)cipherText.c_str(), cipherText.size());
    int plainTextLen = len;

    EVP_DecryptFinal_ex(ctx, plainText.data() + len, &len);
    plainTextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(plainText.begin(), plainText.begin() + plainTextLen);
}
