// December 6th, 2020

#include <cstdlib>
#include <iostream>
#include <string>
#include "exception.hpp"
#include "indexer.hpp"
#include "option_parser.hpp"

using namespace std;

int main(int argc, char **argv) {
    OptionParser options(argc, argv);
    Indexer indexer(options);
    string line;
    long nlines = 0;

    while (getline(cin, line)) {
        try {
            indexer.insert(line);

            if (options.verbose) {
                cout << "\r" << ++nlines;
                cout.flush();
            }
        } catch (Exception &e) {
            cerr << "ERROR: " << e.what() << endl;
        }
    }

    return EXIT_SUCCESS;
}
