// December 7th, 2020

#include "exception.hpp"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cassert>

Exception::Exception(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    assert(vasprintf(&_what, fmt, ap) != -1);

    va_end(ap);
}

Exception::~Exception() {
    free(_what);
}


const char* Exception::what() const noexcept {
    return _what;
}
