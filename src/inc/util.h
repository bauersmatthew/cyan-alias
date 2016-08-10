// utility header, with utility stuff
#ifndef CYALIAS_UTIL_H
#define CYALIAS_UTIL_H

#include <string>
#include <vector>
#include <deque>
#include <exception>

#define CA_DEBUG

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

public:
    CAError(const std::string& message, int line=-1)
    {
        msg = message;
        ln = line;

    }
    const std::string& Message() const
    {
        return msg;
    }
    int Line() const
    {
        return ln;
    }
    const char *what() const
    {
        return message.c_str();
    }
};

#endif
