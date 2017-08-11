/*
 * penergy.h
 *
 *  Created on: 08 Mar 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef ANGULAR_H_
#define ANGULAR_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "msdf.hpp"
#include "particlestream.hpp"

class McfdCommand_angular: public MsdfCommand
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

    double xrmin, xrmax;
    double yrmin, yrmax;
    bool limitX;
    bool limitY;
    bool weightIt;

    double minGamma;
    double maxGamma;
    int dim;

    char xAxisId;
    char yAxisId;

    int64_t chunkLength;

    std::string createOutputFile(int speciesId);
    char makeAxisId(std::string axisStr);
    double getValue(int axis, double px, double py, double pz, double x, double y);

  public:
    McfdCommand_angular();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* PENERGY_H_ */
