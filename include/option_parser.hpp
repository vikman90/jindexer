// December 7th, 2020

#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

#include "database.hpp"
#include <string>

class OptionParser {
public:
    explicit OptionParser(int argc, char* const *argv);

    std::string database_path;
    JournalMode journal_mode;
    bool delete_data;
    bool reindex;
    bool sync;
    bool transaction;
    bool verbose;

private:
    [[noreturn]] void help(const char *argv0);
};

#endif
