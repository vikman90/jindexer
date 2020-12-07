// December 6th, 2020

#ifndef INDEXER_H
#define INDEXER_H

#include "database.hpp"
#include "option_parser.hpp"
#include <cjson/cJSON.h>
#include <string>

class Indexer {
public:
    explicit Indexer(const OptionParser &options);
    ~Indexer();

    void insert(const std::string &line);

private:
    void insert_object(long document_id, cJSON *object, const char *prefix);
    void insert_array(long document_id, cJSON *array, const char *key);
    static char* join(const char *a, const char *b);

    Database database;
    bool must_commit;
    bool must_index;
};

#endif
