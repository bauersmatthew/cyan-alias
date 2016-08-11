// utility header, with utility stuff
#ifndef CYALIAS_UTIL_H
#define CYALIAS_UTIL_H

#include <string>
#include <vector>
#include <deque>
#include <exception>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// for now
#define CA_DEBUG

// useful constants
const std::string PATH_HOME = getenv("HOME") ? std::string(getenv("HOME")) : std::string(getpwuid(getuid())->pw_dir);
const std::string PATH_FILE_REGISTRY = PATH_HOME+"/.cyalias-registry";

template<typename T> T dq_getpop(std::deque<T>& dq, T* p_val=nullptr)
{
    T ret = dq.front();
    dq.pop_front();
    if(p_val)
        *p_val = ret;
    return ret;
}

std::vector<std::string> split_str(const std::string& to_split, char delim);

class CAError : public std::exception
{
private:
    std::string msg;
    int ln;
    std::string func;
    std::string file;
    std::string what_str;

public:
    CAError(const std::string& message, int line, const std::string& func, const std::string& file);
    const std::string& Message() const;
    int Line() const;
    const std::string& Function() const;
    const std::string& File() const;
    const char *what() const noexcept(true);
};
#define CAERR(msg) CAError(msg, __LINE__, __func__, __FILE__)

// report an error in some semi-standard way
void log_err(const std::string& msg);
void log_err(const CAError& err);

#endif
