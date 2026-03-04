/*
 * tohdf.h
 *
 *  Created on: 8 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef JOINSLICES_H_
#define JOINSLICES_H_


#include <boost/program_options.hpp>
#include "../commands.hpp"
#include "../dataio.hpp"

class CfdMeshVariable;

class McfdCommand_joinslices : public MsdfCommand
{
  private:
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;

    std::string outputName;
    std::string metaName;

    bool writeMeta;

    MeshData meshData;

    void writeMeshVariable();
    void writeMeshVariableText();
    void constructOutputFileName() { throw msdf::GenericException("Not implemented yet!"); }
  public:
    McfdCommand_joinslices();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* JOINSLICES_H_ */
