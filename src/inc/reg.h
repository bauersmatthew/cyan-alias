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

    std::string get_path(std::deque<std::string>& alias_accession) const; // protected because should only be used internally

    static enum EditAction
    {
        EA_ADD_GRP,
        EA_REM_GRP,
        EA_ADD_ALI,
        EA_REM_ALI
    };

    void edit(std::deque<std::string>& accession, EditAction action, const std::string& arg2="");

    void load(std::deque<uint8_t>& data);
    void save(std::vector<uint8_t>& data) const;
public:
    AliasGroup(); // load empty
    AliasGroup(std::deque<uint8_t>& data); // load from data
};
class AliasRegistry : public AliasGroup
{
private:
    bool changed;

public:
    AliasRegistry(); // load empty
    AliasRegistry(const std::string& path); // load from file
    void load(const std::string& path);

    std::string get_path(const std::string& alias_accession) const; // get system path from alias path

    void add_group(const std::string& name);
    void remove_group(const std::string& name);

    void add_alias(const std::string& alias, const std::string& path);
    void remove_alias(const std::string& alias);

    void save(const std::string& path) const;
};

void cmd_manage_aliases(const std::vector<std::string>& params, AliasRegistry& alias_reg);

#endif
