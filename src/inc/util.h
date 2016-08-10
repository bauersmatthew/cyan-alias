// utility header, with utility stuff
#ifndef CYALIAS_UTIL_H
#define CYALIAS_UTIL_H

#include <string>
#include <vector>
#include <deque>
#include <exception>
#include <sstream>

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

std::vector<std::string> split_str(const std::string& to_split, char delim)
{
    std::vector<std::string> ret;
    std::string tmp = "";
    for(char ch : to_split)
    {
        if(ch == delim)
        {
            ret.push_back(tmp);
            tmp = "";
        }
        else
            tmp += ch;
    }
    ret.push_back(tmp);
    return ret;
}

class CAError : public std::exception
{
private:
    std::string msg;
    int ln;
    const std::string& func;
    const std::string& file;

public:
    CAError(const std::string& message, int line, const std::string& func, const std::string& file)
    {
        msg = message;
        ln = line;
        this->func = func;
        this->file = file;
    }
    const std::string& Message() const
    {
        return msg;
    }
    int Line() const
    {
        return ln;
    }
    const std::string& Function() const
    {
        return func;
    }
    const std::string& File() const
    {
        return file;
    }
    const char *what() const
    {
        std::stringstream ss;
#ifdef CA_DEBUG
        ss << msg << "(dbg: line " << ln << " in " << func << " in " << file << ")";
#else
        ss << msg;
#endif
        return ss.str().c_str();
    }
};
#define CAERR(msg) CAError(msg, __LINE__, __func__, __FILE__)

// report an error in some semi-standard way
void log_err(const std::string& msg)
{
    std::cerr << "e: " << msg << std::endl;
}
void log_err(const CAError& err)
{
#ifdef CA_DEBUG
    std::cerr << "e: " << err.Message() << "(@ln " << err.Line() << ")\n";
#else
    log_err(err.Message());
#endif
}

#endif
