#include "message_db.h"
#include <iostream>

MessageDB::MessageDB(const std::string &database_name) {
    if (sqlite3_open(database_name.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

MessageDB::~MessageDB() {
    if (db) {
        sqlite3_close(db);
    }
}

void MessageDB::initialize() {
    const char *sql = "CREATE TABLE IF NOT EXISTS Messages (Id INTEGER PRIMARY KEY AUTOINCREMENT, Message TEXT);";
    char *errMsg;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void MessageDB::save_message(const std::string &message) {
    const char *sql = "INSERT INTO Messages (Message) VALUES (?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, message.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        }
    }
    sqlite3_finalize(stmt);
}

void MessageDB::fetch_all_messages() {
    const char *sql = "SELECT Message FROM Messages;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *message = sqlite3_column_text(stmt, 0);
            std::cout << "Fetched Message: " << message << std::endl;
        }
    }
    sqlite3_finalize(stmt);
}
