/*
 * File: base.cpp
 * Edited: 29 Jul 2016
 * Author: Matthew Bauer
 */

// includes
#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <stdlib.h>
#include <stdint.h>

#include "inc/util.h"
#include "inc/reg.h"

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
    catch(CAError& err)
    {
        log_err(err);
        return -1;
    }

    // extract args from argv
    std::string cmd = argv[1];
    std::vector<std::string> params;
    for(char **curr_arg = argv+2; curr_arg != argv + argc; curr_arg++)
        params.push_back(std::string(*curr_arg));

    // switch to command
    if(cmd == "--help")
    {
        print_help(std::cout);
    }
    else if(cmd == "x")
    {
        try
        {
            // translate all params
            std::vector<std::string> trans_params;
            for(auto par : params)
            {
                if(par.find_first_of(':') != std::string::npos)
                {
                    if(par.size() > 1 && par[0] == ':' && par[1] == ':')
                        trans_params.push_back(par); //:: ==> no translation!
                    else
                        trans_params.push_back(alias_reg.get_path(par));
                }
                else
                    trans_params.push_back(par);
            }
            // create command
            std::string sys_cmd = "";
            for(auto tp : trans_params)
                sys_cmd += "\"" + tp + "\" ";
            // call system
            system(sys_cmd.c_str());
        }
        catch(CAError& err)
        {
            log_err(err);
            return -1;
        }
        catch(...)
        {
            log_err("something bad happened... not sure what, though...");
            return -1;
        }
    }
    else if(cmd == "a")
    {
        try
        {
            cmd_manage_aliases(params, alias_reg);
        }
        catch(CAError& err)
        {
            log_err(err);
            return -1;
        }
        catch(...)
        {
            log_err("something bad happened... not sure what, though...");
            return -1;
        }
    }
    else
    {
        log_err("command '" + cmd + "' not recognized");
        print_help(std::cerr);
        return -1;
    }

    return 0;
}
