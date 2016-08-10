// handle alias registry stuff (header)
#ifndef CYALIAS_REG_H
#define CYALIAS_REG_H

#include <deque>
#include <vector>
#include <stdint.h>

class AliasGroup
{
protected:
    std::map<std::string, AliasGroup> subgroups;
    std::map<std::string, std::string> aliases;

    std::string get_path(std::deque<std::string>& alias_accession); // protected because should only be used internally

public:
    AliasGroup(); // load empty
    AliasGroup(std::deque<uint8_t>& data); // load from data
    void load(std::deque<uint8_t>& data);
};
class AliasRegistry : public AliasGroup
{
    AliasRegistry(); // load empty
    AliasRegistry(const std::string& path); // load from file
    void load(const std::string& path);

    std::string get_path(const std::string& alias_accession); // get system path from alias path
};

#endif
