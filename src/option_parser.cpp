// December 7th, 2020

#include "option_parser.hpp"
#include "exception.hpp"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

#define DEFAULT_DB_PATH "var/index.db"

using namespace std;

OptionParser::OptionParser(int argc, char* const *argv) : database_path(DEFAULT_DB_PATH), journal_mode(JOURNAL_DELETE), delete_data(false), reindex(false), sync(true), transaction(false), verbose(false) {
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "dhj:rstv")) != -1) {
        switch (c) {
            case '?':
                throw Exception("Unknown option -%c", c);

            case 'd':
                delete_data = true;
                break;

            case 'h':
                help(argv[0]);

            case 'j':
                if (strcasecmp(optarg, "delete") == 0) {
                    journal_mode = JOURNAL_DELETE;
                } else if (strcasecmp(optarg, "truncate") == 0) {
                    journal_mode = JOURNAL_TRUNCATE;
                } else if (strcasecmp(optarg, "memory") == 0) {
                    journal_mode = JOURNAL_MEMORY;
                } else if (strcasecmp(optarg, "wal") == 0) {
                    journal_mode = JOURNAL_WAL;
                } else if (strcasecmp(optarg, "off") == 0) {
                    journal_mode = JOURNAL_OFF;
                } else {
                    throw Exception("Unknown argument for option -%c", c);
                }

                break;

            case 'r':
                reindex = true;
                break;

            case 's':
                sync = false;
                break;

            case 't':
                transaction = true;
                break;

            case 'v':
                verbose = true;
        }
    }

    if (optind < argc) {
        database_path = argv[optind];
    }
}

void OptionParser::help(const char *argv0) {
    cout << "vIndexer 0.1\n"
            "Syntax: " << argv0 << " [OPTIONS] [INDEX_PATH]\n"
            "    INDEX_PATH     Path to the index database. Default: " DEFAULT_DB_PATH "\n"
            "Options:\n"
            "    -d             Delete all existing data in the index.\n"
            "    -h             Show this help and exit.\n"
            "    -j mode        Journal mode: delete|truncate|memory|wal|off. Default: delete.\n"
            "    -r             Enable re-indexing.\n"
            "    -s             Disable disk synchronization.\n"
            "    -t             Enable transaction.\n"
            "    -v             Verbose mode.\n";

    exit(EXIT_SUCCESS);
}
