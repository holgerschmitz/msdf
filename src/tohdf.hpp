/*
 * tohdf.h
 *
 *  Created on: 8 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef TOHDF_H_
#define TOHDF_H_


#include <boost/program_options.hpp>
#include "commands.hpp"
#include "dataio.hpp"

class CfdMeshVariable;

class McfdCommand_tohdf : public McfdCommand
{
  private:
    boost::program_options::options_description option_desc;
    boost::program_options::positional_options_description option_pos;

    std::string outputName;
    std::string metaName;

    bool writeMeta;

    MeshData meshData;

    void constructOutputFileName();
    void writeMeshVariable();
    void writeMeshVariableText();
  public:
    McfdCommand_tohdf();
    void execute(int argc, char **argv);
    void print_help();
};

#endif /* TOHDF_H_ */
