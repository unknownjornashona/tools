#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>

struct Header {
    char chunkID[4];
    uint32_t chunkSize;
    char format[4];
};

struct Subchunk1 {
    char subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct Subchunk2 {
    char subchunk2ID[4];
    uint32_t subchunk2Size;
};

class AudioSteganography {
private:
    std::string logFilePath;

    void log(const std::string& message) {
        std::ofstream logFile(logFilePath, std::ios::app);
        if (logFile) {
            logFile << message << std::endl;
        }
    }

public:
    AudioSteganography(const std::string& logPath) : logFilePath(logPath) {}

    void encodeMessage(const std::string& message, const std::string& audioPath, const std::string& outputPath) {
        std::ifstream audioFile(audioPath, std::ios::binary);
        if (!audioFile.is_open()) {
            log("Failed to open input audio file.");
            throw std::runtime_error("Failed to open input audio file.");
        }

        Header header;
        Subchunk1 subchunk1;
        Subchunk2 subchunk2;

        audioFile.read(reinterpret_cast<char*>(&header), sizeof(header));
        audioFile.read(reinterpret_cast<char*>(&subchunk1), sizeof(subchunk1));
        audioFile.read(reinterpret_cast<char*>(&subchunk2), sizeof(subchunk2));

        if (subchunk1.bitsPerSample != 16) {
            log("Only 16-bit PCM is supported.");
            throw std::runtime_error("Only 16-bit PCM is supported.");
        }

        std::vector<int16_t> audioSamples(subchunk2.subchunk2Size / sizeof(int16_t));
        audioFile.read(reinterpret_cast<char*>(audioSamples.data()), subchunk2.subchunk2Size);
        audioFile.close();

        size_t messageLength = message.size();
        if (messageLength > (audioSamples.size() / 8)) {
            log("Message is too long to encode.");
            throw std::runtime_error("Message is too long to encode.");
        }

        // Encode message length
        for (size_t i = 0; i < 4; ++i) {
            audioSamples[i] &= 0xFFFE; // Clear LSB
            audioSamples[i] |= ((messageLength >> i) & 1); // Set LSB
        }

        // Encode message
        for (size_t i = 0; i < messageLength; ++i) {
            for (size_t bit = 0; bit < 8; ++bit) {
                audioSamples[i * 8 + 4 + bit] &= 0xFFFE; // Clear LSB
                audioSamples[i * 8 + 4 + bit] |= ((message[i] >> bit) & 1); // Set LSB
            }
        }

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open()) {
            log("Failed to open output audio file.");
            throw std::runtime_error("Failed to open output audio file.");
        }

        outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
        outFile.write(reinterpret_cast<const char*>(&subchunk1), sizeof(subchunk1));
        outFile.write(reinterpret_cast<const char*>(&subchunk2), sizeof(subchunk2));
        outFile.write(reinterpret_cast<const char*>(audioSamples.data()), subchunk2.subchunk2Size);
        outFile.close();
        log("Message encoded successfully.");
    }

    std::string decodeMessage(const std::string& audioPath) {
        std::ifstream audioFile(audioPath, std::ios::binary);
        if (!audioFile.is_open()) {
            log("Failed to open audio file for decoding.");
            throw std::runtime_error("Failed to open audio file for decoding.");
        }

        Header header;
        Subchunk1 subchunk1;
        Subchunk2 subchunk2;

        audioFile.read(reinterpret_cast<char*>(&header), sizeof(header));
        audioFile.read(reinterpret_cast<char*>(&subchunk1), sizeof(subchunk1));
        audioFile.read(reinterpret_cast<char*>(&subchunk2), sizeof(subchunk2));

        if (subchunk1.bitsPerSample != 16) {
            log("Only 16-bit PCM is supported.");
            throw std::runtime_error("Only 16-bit PCM is supported.");
        }

        std::vector<int16_t> audioSamples(subchunk2.subchunk2Size / sizeof(int16_t));
        audioFile.read(reinterpret_cast<char*>(audioSamples.data()), subchunk2.subchunk2Size);
        audioFile.close();

        size_t messageLength = 0;
        for (size_t i = 0; i < 4; ++i) {
            messageLength |= (audioSamples[i] & 1) << i;
        }

        if (messageLength > (audioSamples.size() / 8)) {
            log("Invalid message length.");
            throw std::runtime_error("Invalid message length.");
        }

        std::string message;
        for (size_t i = 0; i < messageLength; ++i) {
            char charValue = 0;
            for (size_t bit = 0; bit < 8; ++bit) {
                charValue |= (audioSamples[i * 8 + 4 + bit] & 1) << bit;
            }
            message += charValue;
        }

        log("Message decoded successfully.");
        return message;
    }
};

int main() {
    try {
        std::string message = "Secret Message"; // 待隐藏消息
        std::string audioPath = "input_audio.wav"; // 输入音频文件路径
        std::string outputPath = "output_audio.wav"; // 输出音频文件路径
        std::string logFilePath = "log.txt"; // 日志文件路径

        AudioSteganography stego(logFilePath);
        stego.encodeMessage(message, audioPath, outputPath);
        std::string decodedMessage = stego.decodeMessage(outputPath);

        std::cout << "Decoded Message: " << decodedMessage << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
