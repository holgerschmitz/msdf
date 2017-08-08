/*
 * penergy.h
 *
 *  Created on: 14 Feb 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef PENERGY_H_
#define PENERGY_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "particlestream.hpp"

class McfdCommand_penergy : public McfdCommand
{
  private:
    typedef DataGrid1d::const_storage_iterator GridIt1;
    typedef DataGrid2d::const_storage_iterator GridIt2;

    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;
    ParticleStreamFactory streamFact;

    std::string mfString;
    std::string massString;
    std::string xsminStr, xsmaxStr;
    std::string ysminStr, ysmaxStr;
    double minGamma;

    void parseNumberList(std::string s, std::vector<double> &v);

  public:
    McfdCommand_penergy();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* PENERGY_H_ */
