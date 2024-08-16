#include "communication.h"

Command parseCommand(const QString& message) {
    Command command = { CommandType::UNKNOWN, "" };

    if (message.startsWith("SEND:")) {
        command.type = CommandType::SEND;
        command.payload = message.mid(5); // 获取消息内容
    } else if (message == "CLOSE") {
        command.type = CommandType::CLOSE;
    }

    return command;
}
