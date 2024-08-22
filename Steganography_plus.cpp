#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fcntl.h>    // For open()
#include <unistd.h>   // For close(), read(), write()
#include <stdexcept>
#include <iomanip>
#include <ctime>
#include <cstring>    // For memset

class Steganography {
private:
    std::string logFilePath;

    void log(const std::string &message) {
        std::ofstream logFile(logFilePath, std::ios::app);
        if (logFile) {
            logFile << getCurrentTime() << " - " << message << std::endl;
        }
    }

    std::string getCurrentTime() {
        auto now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buf;
    }

public:
    Steganography(const std::string &logPath) : logFilePath(logPath) {}

    void hideData(const std::string &devicePath, const std::vector<char> &data, off_t offset) {
        int fd = open(devicePath.c_str(), O_RDWR);
        if (fd < 0) {
            log("Failed to open device.");
            throw std::runtime_error("Failed to open device.");
        }

        if (lseek(fd, offset, SEEK_SET) == -1) {
            log("Failed to seek to offset.");
            close(fd);
            throw std::runtime_error("Failed to seek to offset.");
        }

        ssize_t bytesWritten = write(fd, data.data(), data.size());
        if (bytesWritten < 0) {
            log("Failed to write data to unallocated space.");
            close(fd);
            throw std::runtime_error("Failed to write data to unallocated space.");
        }

        close(fd);
        log("Data hidden successfully.");
    }

    std::vector<char> revealData(const std::string &devicePath, off_t offset, size_t size) {
        std::vector<char> buffer(size);
        int fd = open(devicePath.c_str(), O_RDONLY);
        if (fd < 0) {
            log("Failed to open device.");
            throw std::runtime_error("Failed to open device.");
        }

        if (lseek(fd, offset, SEEK_SET) == -1) {
            log("Failed to seek to offset.");
            close(fd);
            throw std::runtime_error("Failed to seek to offset.");
        }

        ssize_t bytesRead = read(fd, buffer.data(), size);
        if (bytesRead < 0) {
            log("Failed to read data from unallocated space.");
            close(fd);
            throw std::runtime_error("Failed to read data from unallocated space.");
        }

        close(fd);
        log("Data revealed successfully.");
        return buffer;
    }

    bool detectHiddenData(const std::string &devicePath, off_t offset, size_t size) {
        std::vector<char> buffer(size);
        int fd = open(devicePath.c_str(), O_RDONLY);
        if (fd < 0) {
            log("Failed to open device for detection.");
            throw std::runtime_error("Failed to open device for detection.");
        }

        if (lseek(fd, offset, SEEK_SET) == -1) {
            log("Failed to seek to offset.");
            close(fd);
            throw std::runtime_error("Failed to seek to offset.");
        }

        ssize_t bytesRead = read(fd, buffer.data(), size);
        if (bytesRead < 0) {
            log("Failed to read data for detection.");
            close(fd);
            throw std::runtime_error("Failed to read data for detection.");
        }

        // 优化检测逻辑：检查是否有有效数据
        const char pattern = 0x00; // 假设填充的是0x00
        for (size_t i = 0; i < size; ++i) {
            if (buffer[i] != pattern) {
                close(fd);
                log("Hidden data detected.");
                return true; // 找到非零填充的数据
            }
        }

        close(fd);
        log("No hidden data found.");
        return false; // 没有发现隐写数据
    }
};

int main() {
    const std::string devicePath = "/dev/sdX"; // 示例设备路径，需根据系统调整
    const std::string logPath = "log.txt";
    const off_t offset = 1024; // 示例未分配空间的偏移量

    try {
        Steganography steganography(logPath);
        
        // 隐藏数据示例
        std::vector<char> secretData = {'H', 'e', 'l', 'l', 'o'};
        steganography.hideData(devicePath, secretData, offset);

        // 检测是否有隐写数据
        if (steganography.detectHiddenData(devicePath, offset, secretData.size())) {
            std::cout << "Hidden data detected." << std::endl;
        } else {
            std::cout << "No hidden data found." << std::endl;
        }

        // 揭示数据示例
        std::vector<char> revealedData = steganography.revealData(devicePath, offset, secretData.size());
        std::cout << "Revealed Data: ";
        for (char c : revealedData) {
            std::cout << c; // 输出解密的数据
        }
        std::cout << std::endl;

    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
