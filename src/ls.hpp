/*
 * ls.h
 *
 *  Created on: 4 Oct 2010
 *      Author: hschmitz
 */

#ifndef LS_H_
#define LS_H_

#include <boost/program_options.hpp>
#include "commands.hpp"

class McfdCommand_ls : public MsdfCommand
{
  private:
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;

    std::string fileName;
  public:
    McfdCommand_ls();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* LS_H_ */
