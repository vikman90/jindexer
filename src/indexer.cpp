// December 6th, 2020

#include "indexer.hpp"
#include "exception.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

#include <iostream>

using namespace std;

Indexer::Indexer(const OptionParser &options) : database(options.database_path), must_commit(false), must_index(false)  {
    database.set_journal_mode(options.journal_mode);

    if (!options.sync) {
        database.disable_sync();
    }

    if (options.reindex) {
        database.drop_indices();
        must_index = true;
    }

    if (options.transaction) {
        database.begin();
        must_commit = true;
    }

    if (options.delete_data) {
        database.delete_data();
    }
}

Indexer::~Indexer() {
    if (must_commit) {
        database.commit();
    }

    if (must_index) {
        database.index();
    }
}

void Indexer::insert(const string &line) {
    cJSON *root = cJSON_Parse(line.c_str());

    if (root == NULL) {
        throw Exception("This is not a JSON document.");
    }

    if (cJSON_IsObject(root)) {
        long document_id = database.add_document(line.c_str());
        insert_object(document_id, root, NULL);
    } else {
        throw Exception("This is not a JSON object.");
    }

    cJSON_Delete(root);
}

void Indexer::insert_object(long document_id, cJSON *object, const char *prefix) {
    for (cJSON *m = object->child; m != NULL; m = m->next) {
        char *key = prefix ? join(prefix, m->string) : strdup(m->string);

        switch (m->type) {
        case cJSON_False:
            database.add_key(key, "false", document_id);
            break;

        case cJSON_True:
            database.add_key(key, "true", document_id);
            break;

        case cJSON_NULL:
            database.add_key(key, "null", document_id);
            break;

        case cJSON_Number:
            database.add_key(key, m->valuedouble, document_id);
            break;

        case cJSON_String:
            database.add_key(key, m->valuestring, document_id);
            break;

        case cJSON_Array:
            insert_array(document_id, m, key);
            break;

        case cJSON_Object:
            insert_object(document_id, m, key);
        }

        free(key);
    }
}

void Indexer::insert_array(long document_id, cJSON *array, const char *key) {
    for (cJSON *m = array->child; m != NULL; m = m->next) {
        switch (m->type) {
        case cJSON_False:
            database.add_key(key, "false", document_id);
            break;

        case cJSON_True:
            database.add_key(key, "true", document_id);
            break;

        case cJSON_NULL:
            database.add_key(key, "null", document_id);
            break;

        case cJSON_Number:
            database.add_key(key, m->valuedouble, document_id);
            break;

        case cJSON_String:
            database.add_key(key, m->valuestring, document_id);
            break;

        case cJSON_Array:
            insert_array(document_id, m, key);
        }
    }
}

char* Indexer::join(const char *a, const char *b) {
    size_t len_a = strlen(a);
    char *string = (char *)malloc(len_a + strlen(b) + 2);

    strcpy(string, a);
    string[len_a] = '.';
    strcpy(string + len_a + 1, b);

    return string;
}
