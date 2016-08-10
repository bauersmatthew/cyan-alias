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
#include <sstream>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <stdlib.h>
#include <stdint.h>

#include "inc/util.h"
#include "inc/reg.h"

// constants
const std::string PATH_HOME = getenv("HOME") ? std::string(getenv("HOME")) : std::string(getpwuid(getuid())->pw_dir);
const std::string PATH_FILE_REGISTRY = PATH_HOME+"/.cyalias-registry";

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

// print basic help dialogue
void print_help(std::ostream& dest)
{
    dest
        << "usage: cy-alias <command> [options]\n"
        << "details:\n"
        << "\tcommand\tthe sub-command for cy-alias to launch\n"
        << "\t\tx\texecute command\n"
        << "\t\ta\tmanage aliases\n"
        << "\toptions\tthe arguments required by the sub-command\n"
        << "more info coming soon...\n";
}

int main(int argc, char **argv)
{
    // check arg #
    if(argc < 2)
    {
        // no args given
        log_err("no arguments passed");
        print_help(std::cerr);
        return -1;
    }

    // load alias registry
    AliasRegistry alias_reg;
    try
    {
        alias_reg.load(PATH_FILE_REGISTRY);
    }
    catch(const std::string& err)
    {
        log_err(err);
        return -1;
    }

    // extract args from argv
    std::string cmd = argv[1];
    std::vector<std::string> params;
    for(char *curr_arg = argv[2]; curr_arg != argv + argc; curr_arg++)
        params.push_back(std::string(curr_arg));

    // switch to command
    if(cmd == "--help")
    {
        print_help(std::cout);
    }
    else if(cmd == "x")
    {
        // TODO
    }
    else if(cmd == "a")
    {
        // TODO
    }
    else
    {
        log_err("command '" + cmd + "' not recognized");
        print_help(std::cerr);
        return -1;
    }

    return 0;
}
