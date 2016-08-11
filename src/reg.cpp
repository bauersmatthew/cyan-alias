// handle alias registry stuff
#include "inc/reg.h"

#include <iostream>

// rethrow if CAError; throw new one otherwise
#define RETHROW_SWITCH(msg) catch(CAError&){throw;}catch(...){throw CAERR(msg);}

// AliasGroup defs
AliasGroup::AliasGroup() {}
AliasGroup::AliasGroup(std::deque<uint8_t>& data)
{
    load(data);
}
void AliasGroup::load(std::deque<uint8_t>& data)
{
    try
    {
        uint8_t num_subels = dq_getpop(data); // includes both subgroups and (sub)aliases
        while(num_subels--)
        {
            // read through subels (cse = current sub element)
            // get subel type (1B), 1 --> alias, 2 --> group
            uint8_t cse_type = dq_getpop(data);
            if(cse_type == 1)
            {
                std::string alias = "";
                std::string path = "";

                // load alias until \0
                uint8_t ch;
                while(dq_getpop(data, &ch))
                    alias += (char)ch;

                // load path until \0
                while(dq_getpop(data, &ch))
                    path += (char)ch;

                // stick in map
                aliases[alias] = path;
            }
            else if(cse_type == 2)
            {
                // load name until \0
                std::string grp_name = "";
                uint8_t ch;
                while(dq_getpop(data, &ch))
                    grp_name += (char)ch;

                // create subgroup, pass datastream to it
                subgroups[grp_name] = AliasGroup(data);
            }
            else
            {
                throw CAERR("invalid data");
            }
        }
    }
    RETHROW_SWITCH("invalid data");
}
std::string AliasGroup::get_path(std::deque<std::string>& alias_accession) const
{
    try
    {
        if(alias_accession.size() == 1)
            return aliases[alias_accession.front()];
        else
            return subgroups[dq_getpop(alias_accession)].get_path(alias_accession);
    }
    RETHROW_SWITCH("invalid alias");
}

void AliasGroup::edit(std::deque<std::string>& accession, EditAction action, const std::string& arg2)
{
    // sub-function definitions
    auto lf_add_group = [this->subgroups, accession]() {this->subgroups[accession.front()] = AliasGroup();};
    auto lf_rem_group = [this->subgroups, accession]() {this->subgroups.erase(accession.front());};
    auto lf_add_alias = [this->aliases, accession, arg2]() {this->aliases[accession.front()] = arg2;};
    auto lf_rem_alias = [this->aliases, accession]() {this->aliases.erase(accession.front());};

    // choose sub-function
    auto lf_use = lf_add_group;
    if(action == EA_ADD_GRP)
        lf_use = lf_add_group;
    else if(action == EA_REM_GRP)
        lf_use = lf_rem_group;
    else if(action == EA_ADD_ALI)
        lf_use = lf_add_alias;
    else if(action == EA_REM_ALI)
        lf_use = lf_add_alias;
    else
        throw CAERR("invalid edit action");

    try
    {
        if(accession.size() == 1)
            lf_use();
        else
            subgroups[dq_getpop(accession)].edit(accession, action, arg2);
    }
    RETHROW_SWITCH("invalid name/accession; failed to edit");
}

void AliasGroup::save(std::vector<uint8_t>& data) const
{
    // write group size
    data.push_back(uint8_t(subgroups.size() + aliases.size()));
    // write aliases
    for(auto al_it = aliases.begin(); al_it != aliases.end(); al_it++)
    {
        data.push_back(1); // 1 = alias identifier
        // write alias name
        for(ch : al_it->first)
            data.push_back((uint8_t)ch);
        data.push_back(0);
        // write path
        for(ch : al_it->second)
            data.push_back((uint8_t)ch);
        data.push_back(0);
    }
    // write groups
    for(auto grp_it = subgroups.begin(); grp_it != subgroups.end(); grp_it++)
    {
        data.push_back(2); // 2 = group identifier
        // write group name
        for(ch : grp_it->first)
            data.push_back((uint8_t)ch);
        // write group contents
        grp_it->second.save(data);
    }
}

// AliasRegistry defs
AliasRegistry::AliasRegistry()
{
    changed = false;
}
AliasRegistry::AliasRegistry(const std::string& path)
{
    changed = false;
    load(path);
}
void AliasRegistry::load(const std::string& path)
{
    std::deque<uint8_t> data;

    // open file
    std::ifstream fin(path, std::ios::bin | std::ios::in);
    if(!fin)
        throw CAERR("could not open registry");

    // read file
    char ch;
    fin.get(ch);
    while(fin)
    {
        data.push_back((uint8_t)ch);
        fin.get(ch);
    }
    fin.close();

    // load as top group using inherited data processing func
    load(data);
}
std::string AliasRegistry::get_path(const std::string& alias_accession) const
{
    try
    {
        if(alias_accession[0] == ':')
            return aliases[alias_accession.substr(1)];
        else
        {
            auto split = split_str(alias_accession, ':');
            std::deque<std::string> new_aa(split.begin(), split.end());
            return get_path(new_aa);
        }
    }
    RETHROW_SWITCH("invalid alias");
}

void AliasRegistry::add_group(const std::string& name)
{
    auto split = split_str(name, ':');
    std::deque<std::string> group_accession(split.begin(), split.end());
    edit(group_accession, EA_ADD_GRP);
    changed = true;
}
void AliasRegistry::remove_group(const std::string& name)
{
    auto split = split_str(name, ':');
    std::deque<std::string> group_accession(split.begin(), split.end());
    edit(group_accession, EA_REM_GRP);
    changed = true;
}
void AliasRegistry::add_alias(const std::string& name, const std::string& path)
{
    try
    {
        if(name[0] == ':')
            aliases[name.substr(1)] = path;
        else
        {
            auto split = split_str(name, ':');
            std::deque<std::string> alias_accession(split.begin(), split.end());
            edit(alias_accession, EA_ADD_ALI, path);
        }
        changed = true;
    }
    RETHROW_SWITCH("failed to add alias");
}
void AliasRegistry::remove_alias(const std::string& name)
{
    try
    {
        if(name[0] == ':')
            aliases.erase(name.substr(1));
        else
        {
            auto split = split_str(name, ':');
            std::deque<std::string> alias_accession(split.begin(), split.end());
            edit(alias_accession, EA_REM_ALI);
        }
        changed = true;
    }
    RETHROW_SWITCH("failed to remove alias");
}
void AliasRegistry::save(const std::string& path) const
{
    if(!changed)
        return;
    // open file
    std::ofstream fout(path, std::ios::bin | std::ios::out);
    if(!fout)
        throw CAERR("could not save alias registry");
    // get data to write
    std::vector<uint8_t> data;
    save(data);
    // write to file
    for(auto byte : data)
        fout.put((char)byte);
    // wrap up
    fout.close();
}

/////////////////
void print_alias_management_help(std::ostream& dest)
{
    dest
        << "usage: cy-alias a <sub-command> [options]\n"
        << "details:\n"
        << "\tsub-command\tthe sub-sub-command passed to cy-alias a\n"
        << "\t\thelp|h\tprint help\n"
        << "\t\tadd|a\tadd alias or group\n"
        << "\t\trem|r\tremove alias or group\n"
        << "\t\tlist|l\tlist alias tree structure\n";
}

#define INV_A_USAGE(spec) \
    {log_err("invalid usage of 'a' command");\
    print_alias_management_help(std::cerr);\
    throw CAERR(spec);}
#define T_INV_A_USAGE(exp, spec) \
    if(exp) INV_A_USAGE(spec)

void cmd_manage_aliases(const std::vector<std::string>& params, AliasRegistry& alias_reg)
{
    T_INV_A_USAGE(params.empty(), "not enough arguments");

    std::string subcmd = params[0];
    if(subcmd == "help" || subcmd == "h")
    {
        print_alias_management_help(std::cout);
    }
    else if(subcmd == "add" || subcmd == "a")
    {
        T_INV_A_USAGE(params.size() < 3, "not enough arguments");

        std::string type = params[1];
        if(type == "alias" || type == "a")
        {
            T_INV_A_USAGE(params.size() < 4, "not enough arguments");
            T_INV_A_USAGE(params.size()%2 != 0, "invalid usage");

            for(int curr = 2; curr < params.size(); curr += 2)
            {
                alias_reg.add_alias(params[curr], params[curr+1]);
            }
        }
        else if(type == "group" || type == "g")
        {
            for(int curr = 2; curr < params.size(); curr++)
            {
                alias_reg.add_group(params[curr]);
            }
        }
        else
        {
            INV_A_USAGE(("type '" + type + "' not recognized"));
        }
    }
    else if(subcmd == "rem" || subcmd == "r")
    {
        T_INV_A_USAGE(params.size() < 3, "not enough arguments");

        std::string type = params[1];
        if(type == "alias" || type == "a")
        {
            for(int curr = 2; curr < params.size(); curr++)
            {
                alias_reg.remove_alias(params[curr]);
            }
        }
        else if(type == "group" || type == "g")
        {
            for(int curr = 2; curr < params.size(); curr++)
            {
                alias_reg.remove_group(params[curr]);
            }
        }
        else
        {
            INV_A_USAGE(("type '" + type + "' not recognized"));
        }
    }
    else if(subcmd == "list" || subcmd == "l")
    {
        //TODO
        throw CAERR("list function not yet supported D:");
    }
    else
    {
        INV_A_USAGE(("sub-sub-command '" + subcmd + "' not recognized"))
    }

    alias_reg.save();
}
