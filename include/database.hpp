// December 6th, 2020

#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

enum Statement {
    STMT_INSERT_DOCUMENT,
    STMT_INSERT_KEY_STRING,
    STMT_INSERT_KEY_NUMBER,
    STMT_DELETE_DOCUMENT,
    STMT_DELETE_INDEXER,
    STMT_BEGIN,
    STMT_COMMIT,
    STMT_CREATE_INDEX_KEY,
    STMT_CREATE_INDEX_VALUE_STRING,
    STMT_CREATE_INDEX_VALUE_NUMBER,
    STMT_DROP_INDEX_KEY,
    STMT_DROP_INDEX_VALUE_STRING,
    STMT_DROP_INDEX_VALUE_NUMBER,
    STMT_PRAGMA_SYNC_OFF,
    STMT_PRAGMA_JOURNAL_DELETE,
    STMT_PRAGMA_JOURNAL_TRUNCATE,
    STMT_PRAGMA_JOURNAL_MEMORY,
    STMT_PRAGMA_JOURNAL_WAL,
    STMT_PRAGMA_JOURNAL_OFF,
    STMT_SIZE
};

enum JournalMode {
    JOURNAL_DELETE,
    JOURNAL_TRUNCATE,
    JOURNAL_MEMORY,
    JOURNAL_WAL,
    JOURNAL_OFF
};

class Database {
public:
    explicit Database(const std::string &path);
    ~Database();

    long add_document(const char *data);
    void add_key(const char *key, const char *value, long document_id);
    void add_key(const char *key, double value, long document_id);
    void delete_data();

    void begin();
    void commit();
    void index();
    void drop_indices();
    void disable_sync();
    void set_journal_mode(JournalMode mode);

private:
    sqlite3_stmt* get_stmt(Statement s);

    sqlite3 *db;
    sqlite3_stmt *stmt[STMT_SIZE];
};

#endif
