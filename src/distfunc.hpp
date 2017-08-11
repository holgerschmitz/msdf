/*
 * distfunc.h
 *
 *  Created on: 27 Sep 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef DISTFUNC_H_
#define DISTFUNC_H_

#include <boost/program_options.hpp>
#include "commands.hpp"
#include "msdf.hpp"
#include "particlestream.hpp"

class McfdCommand_distfunc: public McfdCommand
{
  private:
    typedef DataGrid1d::const_storage_iterator GridIt1;
    typedef DataGrid2d::const_storage_iterator GridIt2;
    
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;
    ParticleStreamFactory streamFact;

    std::string outputName;
    std::string axis;
    std::string moment;

    double xrmin, xrmax;
    double yrmin, yrmax;
    double dmin, dmax;
    bool limitX;
    bool limitY;

    double minGamma;
    double maxGamma;
    double lfactor;
    int dim;

    char axisId, momentId;

    std::string createOutputFile(int speciesId);
    char makeAxisId(std::string axisStr, bool allowUnity=false);
    double getValue(int axis, double px, double py, double pz, double x, double y);

  public:
    McfdCommand_distfunc();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* PENERGY_H_ */
