#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QString>

enum class CommandType {
    SEND,
    CLOSE,
    UNKNOWN
};

struct Command {
    CommandType type;
    QString payload;
};

Command parseCommand(const QString& message);

#endif // COMMUNICATION_H
