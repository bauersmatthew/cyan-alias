// utility header, with utility stuff
#ifndef CYALIAS_UTIL_H
#define CYALIAS_UTIL_H

#include <string>
#include <vector>
#include <deque>

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

#endif
