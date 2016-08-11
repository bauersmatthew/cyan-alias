/*
 * File: util.cpp
 * Edited: 11 Aug 2016
 * Author: Matthew Bauer
 */

#include "inc/util.h"
#include <iostream>
#include <sstream>

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

CAError::CAError(const std::string& message, int line, const std::string& func, const std::string& file)
{
    msg = message;
    ln = line;
    this->func = func;
    this->file = file;
    
    std::stringstream ss;
    ss << msg << " (dbg: line " << ln << " in " << func << " in " << file << ")";
    what_str = ss.str();
}

const std::string& CAError::Message() const
{
    return msg;
}
int CAError::Line() const
{
    return ln;
}
const std::string& CAError::Function() const
{
    return func;
}
const std::string& CAError::File() const
{
    return file;
}
const char *CAError::what() const noexcept(true)
{
    return what_str.c_str();
}

void log_err(const std::string& msg)
{
    std::cerr << "e: " << msg << std::endl;
}
void log_err(const CAError& err)
{
    log_err(std::string(err.what()));
}
