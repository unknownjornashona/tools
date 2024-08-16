#ifndef MESSAGE_DB_H
#define MESSAGE_DB_H

#include <sqlite3.h>
#include <string>

class MessageDB {
public:
    MessageDB(const std::string &database_name);
    ~MessageDB();

    void initialize();
    void save_message(const std::string &message);
    void fetch_all_messages();

private:
    sqlite3 *db;
};

#endif // MESSAGE_DB_H
