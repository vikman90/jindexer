// December 7th, 2020

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

class Exception : public std::exception {
public:
    explicit Exception(const char *fmt, ...);
    ~Exception();

    const char* what() const noexcept;

private:
    char *_what;
};

#endif
