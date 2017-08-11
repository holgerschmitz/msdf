/*
 * penergy.h
 *
 *  Created on: 08 Mar 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef PHASEPLOT_H_
#define PHASEPLOT_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "msdf.hpp"
#include "particlestream.hpp"

class McfdCommand_phaseplot: public McfdCommand
{
  private:
    typedef DataGrid1d::const_storage_iterator GridIt1;
    typedef DataGrid2d::const_storage_iterator GridIt2;
    
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;
    ParticleStreamFactory streamFact;

    std::string outputName;
    std::string xAxis;
    std::string yAxis;
    std::string moment;
    double minGamma;
    double maxGamma;
    int xdim, ydim;

    char xAxisId;
    char yAxisId;
    char momentId;
    std::string xrminStr, xrmaxStr;
    std::string yrminStr, yrmaxStr;
    std::string xsminStr, xsmaxStr;
    std::string ysminStr, ysmaxStr;
    int limitX;
    int limitY;

    bool batch;

    int64_t chunkLength;

    std::string createOutputFile(int speciesId);
    char makeAxisId(std::string axisStr, bool allowUnity=false);
    double getValue(int axis, double px, double py, double pz, double x, double y);
    void parseNumberList(std::string s, std::vector<double> &v);

  public:
    McfdCommand_phaseplot();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* PENERGY_H_ */
