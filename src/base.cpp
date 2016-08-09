/*
 * File: base.cpp
 * Edited: 29 Jul 2016
 * Author: Matthew Bauer
 */

// includes
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <stack>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <stdlib.h>
#include <stdint.h>

// constants
const std::string PATH_HOME = getenv("HOME") ? std::string(getenv("HOME")) : std::string(getpwuid(getuid())->pw_dir);
const std::string PATH_FILE_REGISTRY = PATH_HOME+"/.cyorg";

// split string
std::vector<std::string> split_str(const std::string& to_split, char delim)
{
    std::vector<std::string> ret; // to return
    std::string curr = ""; // curr group
    for(ch : to_split)
    {
        if(ch == delim)
        {
            ret.push_back(curr);
            curr = "";
        }
        else
            curr += ch;
    }
    ret.push_back(curr);
    return ret;
}
std::string combine_strs(const std::vector<std::string>& to_combine, bool use_delim=false, char delim=0)
{
    std::string ret = "";
    for(str : to_combine)
    {
        if(use_delim)
        {
            ret += str;
            ret += delim;
        }
        else
            ret += str;
    }
    if(use_delim)
        ret = ret.substr(0, ret.size()-1); // remove trailing delim
    return ret;
}
template<typename T> std::vector<T> splice_vector(const std::vector<T>& to_splice, int first, int last=-1)
{
    int tssz = (int)to_splice.size();
    if(last == -1)
        last = tssz-1;
    if(first > tssz-1 || last > tssz-1 || first > last || last < 0)
        throw std::runtime_error("vec splice request out of bounds");
    std::vector<T> ret;
    for(int curr_el = first; curr_el <= last; curr_el++)
        ret.push_back(to_splice[curr_el])
    return ret;
}

// report an error in some semi-standard way
void log_err(const std::string& msg)
{
    std::cerr << "e: " << msg << std::endl;
}

// registry structures
class FLRegGroup
{
private:
    std::map<std::string, FLRegGroup> subgroups;
    std::map<std::string, std::string> aliases;
public:
    std::string get_path(const std::string& alias)
    {
        auto subels = split_str(alias, ':');
        try
        {
            if(subels.len() == 1)
                return aliases[subels[0]]
            else
                return subgroups[subels[0]].get_path(combine_strs(splice_vector(subels, 1), true, ':')); // horribly inefficient... oh well...
        }
        catch(...)
        {
            throw;
        }
    }
    
    friend FLRegGroup load_file_registry();
};
typedef FLRegGroup FLReg;

// load the file link registry
// if file can't be loaded assume file doesn't exist; return empty map
// file registry format: name1\a:b?c:d\name2\w:x?e:f?y:z .......
FLReg load_file_registry()
{
    FLReg ret(); // to fill & return
    
    // load file
    std::ifstream fin(PATH_FILE_REGISTRY, std::ios::in | std::ios::bin);
    if(!fin)
        return ret;
        
    // tmp loop vars
    uint16_t read_scope = 0; // 0 ==> reading group name, 1 ==> reading group contents
    std::stack<std::string> entry_stack;
    
    std::string tmp_strs[3] = {"", "", ""}; // ent_name, 
    std::string cgrp_name = "";
    std::string cent_alias = "";
    std::string cent_path = "";
    std::string *growing = &cgrp_name;
    std::map<std::string, std::string> cgrp_contents;
    
    // prime loop
    char ch;
    fin.get(ch);
    // read loop
    while(fin)
    {
        int old_rs = read_scope; // save read scope at beginning
        if(read_scope == 1)
        {
            if(ch == '\\')
                read_scope = 2;
        }
        else // read_scope == 2
        {
            if(ch == '\\')
                read_scope = 1;
            else if(ch == 
        }
    }
    fin.close();
    return ret;
}

// sub-command functions
#define CMDF(n) int n(const std::vector<std::string>& args)
CMDF(cmd_execute)
{
    // load file registry
    freg = load_file_registry();

    std::string sh_cmd = ""; // command to eventually pass to the shell
    // process arguments
    for(arg : args)
    {
        for(ch : arg)
        {
            if(ch != ':')
                sh_cmd += ch;
            
        }
    }
}
CMDF(


int main(int argc, char **argv)
{
    if(argc == 2 && !strcmp(argv[1], "--help"))
    {
        // print help dialogue
        std::cout
            << "usage: org <command> [options]\n"
            << "details:\n"
            << "\tcommand\tthe sub-command for org to launch\n"
            << "\t\tx\texecute\n"
            << "\t\tr\tregister\n"
            << "\t\tc\tcomment\n"
            << "\toptions\tthe arguments required by the given sub-command\n"
            << "for more info, read the man page (coming soon)\n";
        return 0;
    }
    if(argc < 2 || strlen(argv[1]) != 1)
    {
        // no cmd given
        log_err("invalid usage; use --help for more info");
        return -1;
    }
    
    // prep for branching into proper cmd
    char cmd = argv[1][0]; // command letter
    std::vector<std::string> cmd_args; // arguments to sub-command
    for(char **curr = argv+2; curr != argv+argc; curr++) // fill cmd_args
    {
        cmd_args.push_back(std::string(*curr));
    }
    int (*cmd_func)(const std::vector&); // sub-command function to call
    switch(cmd) // choose correct function
    {
        case 'x':
            cmd_func = cmd_execute;
            break;
        case 'r':
            cmd_func = cmd_register;
            break;
        case 'c':
            cmd_func = cmd_comment;
            break;
    }
    return cmd_func(cmd_args);
}
