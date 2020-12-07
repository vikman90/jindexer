// December 6th, 2020

#include "database.hpp"
#include "exception.hpp"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <string>

using namespace std;

static const char *QUERIES[] = {
    [STMT_INSERT_DOCUMENT] = "INSERT INTO document (data) VALUES (?)",
    [STMT_INSERT_KEY_STRING] = "INSERT INTO indexer (key, value_string, document_id) VALUES (?, ?, ?)",
    [STMT_INSERT_KEY_NUMBER] = "INSERT INTO indexer (key, value_number, document_id) VALUES (?, ?, ?)",
    [STMT_DELETE_DOCUMENT] = "DELETE FROM document",
    [STMT_DELETE_INDEXER] = "DELETE FROM indexer",
    [STMT_BEGIN] = "BEGIN",
    [STMT_COMMIT] = "COMMIT",
    [STMT_CREATE_INDEX_KEY] = "CREATE INDEX IF NOT EXISTS indexer_key ON indexer (key)",
    [STMT_CREATE_INDEX_VALUE_STRING] = "CREATE INDEX IF NOT EXISTS indexer_value_string ON indexer (value_string)",
    [STMT_CREATE_INDEX_VALUE_NUMBER] = "CREATE INDEX IF NOT EXISTS indexer_value_number ON indexer (value_number)",
    [STMT_DROP_INDEX_KEY] = "DROP INDEX IF EXISTS indexer_key",
    [STMT_DROP_INDEX_VALUE_STRING] = "DROP INDEX IF EXISTS indexer_value_string",
    [STMT_DROP_INDEX_VALUE_NUMBER] = "DROP INDEX IF EXISTS indexer_value_number",
    [STMT_PRAGMA_SYNC_OFF] = "PRAGMA synchronous = OFF",
    [STMT_PRAGMA_JOURNAL_DELETE] = "PRAGMA journal_mode = DELETE",
    [STMT_PRAGMA_JOURNAL_TRUNCATE] = "PRAGMA journal_mode = TRUNCATE",
    [STMT_PRAGMA_JOURNAL_MEMORY] = "PRAGMA journal_mode = MEMORY",
    [STMT_PRAGMA_JOURNAL_WAL] = "PRAGMA journal_mode = WAL",
    [STMT_PRAGMA_JOURNAL_OFF] = "PRAGMA journal_mode = OFF",
};

Database::Database(const string &path) : db(NULL), stmt { NULL } {
    if (sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK) {
            Exception e("Cannot open index '%s': %s", path.c_str(), sqlite3_errmsg(db));
            sqlite3_close(db);
            throw e;
    }

    for (int i = 0; i < STMT_SIZE; i++) {
        if (sqlite3_prepare_v2(db, QUERIES[i], -1, &stmt[i], NULL) != SQLITE_OK) {
            Exception e("Preparing statement '%s': %s", QUERIES[i], sqlite3_errmsg(db));
            sqlite3_close(db);
            throw e;
        }
    }
}

Database::~Database() {
    for (int i = 0; i < STMT_SIZE; i++) {
        assert(sqlite3_finalize(stmt[i]) == SQLITE_OK);

    }

    sqlite3_close(db);
}

long Database::add_document(const char *data) {
    sqlite3_stmt *stmt = get_stmt(STMT_INSERT_DOCUMENT);
    assert(sqlite3_bind_blob(stmt, 1, data, strlen(data), NULL) == SQLITE_OK);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
    return sqlite3_last_insert_rowid(db);
}

void Database::add_key(const char *key, const char *value, long document_id) {
    sqlite3_stmt *stmt = get_stmt(STMT_INSERT_KEY_STRING);
    sqlite3_bind_text(stmt, 1, key, -1, NULL);
    sqlite3_bind_text(stmt, 2, value, -1, NULL);
    sqlite3_bind_int64(stmt, 3, document_id);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

void Database::add_key(const char *key, double value, long document_id) {
    sqlite3_stmt *stmt = get_stmt(STMT_INSERT_KEY_NUMBER);
    sqlite3_bind_text(stmt, 1, key, -1, NULL);
    sqlite3_bind_double(stmt, 2, value);
    sqlite3_bind_int64(stmt, 3, document_id);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

void Database::delete_data() {
    sqlite3_stmt *stmt = get_stmt(STMT_DELETE_INDEXER);
    assert(sqlite3_step(stmt) == SQLITE_DONE);

    stmt = get_stmt(STMT_DELETE_DOCUMENT);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

void Database::begin() {
    sqlite3_stmt *stmt = get_stmt(STMT_BEGIN);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

void Database::commit() {
    sqlite3_stmt *stmt = get_stmt(STMT_COMMIT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        throw Exception("Running '%s': %s", QUERIES[STMT_COMMIT], sqlite3_errmsg(db));
    }
}

void Database::index() {
    sqlite3_stmt *stmt = get_stmt(STMT_CREATE_INDEX_KEY);
    assert(sqlite3_step(stmt) == SQLITE_DONE);

    stmt = get_stmt(STMT_CREATE_INDEX_VALUE_STRING);
    assert(sqlite3_step(stmt) == SQLITE_DONE);

    stmt = get_stmt(STMT_CREATE_INDEX_VALUE_NUMBER);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

void Database::drop_indices() {

    if (sqlite3_step(get_stmt(STMT_DROP_INDEX_KEY)) != SQLITE_DONE) {
        throw Exception("Running '%s': %s", QUERIES[STMT_DROP_INDEX_KEY], sqlite3_errmsg(db));
    }

    if (sqlite3_step(get_stmt(STMT_DROP_INDEX_VALUE_STRING)) != SQLITE_DONE) {
        throw Exception("Running '%s': %s", QUERIES[STMT_DROP_INDEX_VALUE_STRING], sqlite3_errmsg(db));
    }

    if (sqlite3_step(get_stmt(STMT_DROP_INDEX_VALUE_NUMBER)) != SQLITE_DONE) {
        throw Exception("Running '%s': %s", QUERIES[STMT_DROP_INDEX_VALUE_NUMBER], sqlite3_errmsg(db));
    }
}

void Database::disable_sync() {
    sqlite3_stmt *stmt = get_stmt(STMT_PRAGMA_SYNC_OFF);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

#include <iostream>

void Database::set_journal_mode(JournalMode mode) {
    sqlite3_stmt *stmt = NULL;

    switch (mode) {
    case JOURNAL_DELETE:
        stmt = get_stmt(STMT_PRAGMA_JOURNAL_DELETE);
        break;
    case JOURNAL_TRUNCATE:
        stmt = get_stmt(STMT_PRAGMA_JOURNAL_TRUNCATE);
        break;
    case JOURNAL_MEMORY:
        stmt = get_stmt(STMT_PRAGMA_JOURNAL_MEMORY);
        break;
    case JOURNAL_WAL:
        stmt = get_stmt(STMT_PRAGMA_JOURNAL_WAL);
        break;
    case JOURNAL_OFF:
        stmt = get_stmt(STMT_PRAGMA_JOURNAL_OFF);
        break;
    default:
        throw Exception("Cannot select journal mode %d.", mode);
    }

    assert(sqlite3_step(stmt) == SQLITE_ROW);
    assert(sqlite3_step(stmt) == SQLITE_DONE);
}

sqlite3_stmt* Database::get_stmt(Statement s) {
    assert(sqlite3_reset(stmt[s]) == SQLITE_OK);
    assert(sqlite3_clear_bindings(stmt[s]) == SQLITE_OK);

    return stmt[s];
}
