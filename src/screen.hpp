/*
 * screen.h
 *
 *  Created on: 4 Jul 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "particlestream.hpp"
#include "mcfd.hpp"

class McfdCommand_screen: public McfdCommand
{
  private:
    typedef DataGrid1d::const_storage_iterator GridIt1;
    typedef DataGrid2d::const_storage_iterator GridIt2;

    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;
    ParticleStreamFactory streamFact;

    std::string outputName;
    std::string moment;
    double minGamma;
    double maxGamma;
    int dim;
    double xscreen;

    char momentId;
    std::string yrminStr, yrmaxStr;
    std::string xsminStr, xsmaxStr;
    std::string ysminStr, ysmaxStr;
    int limitX;

    bool batch;

    std::string createOutputFile(int speciesId);
    char makeAxisId(std::string axisStr);
    double getValue(double px, double py, double pz, double x, double y);
    void parseNumberList(std::string s, std::vector<double> &v);

  public:
    McfdCommand_screen();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* SCREEN_H_ */
