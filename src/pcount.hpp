/*
 * pcount.h
 *
 *  Created on: 19 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef PCOUNT_H_
#define PCOUNT_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "particlestream.hpp"

class McfdCommand_pcount : public MsdfCommand
{
  private:
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;
    ParticleStreamFactory streamFact;


  public:
    McfdCommand_pcount();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* PCOUNT_H_ */
