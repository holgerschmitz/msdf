/*
 * tohdf.cpp
 *
 *  Created on: 8 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */


#include "tohdf.hpp"
#include "hdfstream.hpp"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

McfdCommand_tohdf::McfdCommand_tohdf()
  : option_desc("Options for the 'tohdf' command")
{
  option_desc.add_options()
      ("output,o", po::value<std::string>(&outputName),"name of the hdf file (default: same as the input file")
      ("meta,m", po::value<std::string>(&metaName),"if specified, the name of the file to which to write the meta data")
      ("text,t","write output in ascii text format for gnuplot instead of hdf5");

  meshData.setProgramOptions(option_desc, option_pos);

  option_pos.add("output", 3);
}

void McfdCommand_tohdf::execute(int argc, char **argv)
{

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  if (!meshData.isValid(vm))
  {
    print_help();
    exit(-1);
  }

  if (vm.count("output")<1)
  {
    this->constructOutputFileName();
  }

  writeMeta = (vm.count("meta")>0);

  meshData.readData(vm);

  if (vm.count("text")<1)
    this->writeMeshVariable();
  else
    this->writeMeshVariableText();

  if (writeMeta)
  {
    std::ofstream meta(metaName.c_str(), std::ofstream::app);
    meta << outputName << " " << meshData.getBlockName() << " " << meshData.getMin(0) << " " << meshData.getMax(0) << "\n";
    meta.close();
  }

}

void McfdCommand_tohdf::constructOutputFileName()
{
  outputName = fs::path(meshData.getInputName()).replace_extension("h5").string();
  std::cout << "Output name is " << outputName << "\n";
}

void McfdCommand_tohdf::writeMeshVariable()
{
  HDFostream output(outputName.c_str());
  output.setBlockName(meshData.getBlockName());

  switch (meshData.getRank())
  {
    case 1:
      output << *(meshData.get1dMesh(0));
      break;
    case 2:
      output << *(meshData.get2dMesh(0));
      break;
    case 3:
      output << *(meshData.get3dMesh(0));
      break;
  }

  output.close();
}


void McfdCommand_tohdf::writeMeshVariableText()
{
  std::ofstream output(outputName.c_str());

  switch (meshData.getRank())
  {
    case 1:
    {
      DataGrid1d &m = *(meshData.get1dMesh(0));
      GridIndex1d l = m.getLo(), h = m.getHi();
      for (int i=l[0]; i<=h[0]; ++i)
        output << i << " " << m(i) << std::endl;
    }
    break;
    case 2:
    {
      DataGrid2d &m = *(meshData.get2dMesh(0));
      GridIndex2d l = m.getLo(), h = m.getHi();

      for (int i=l[0]; i<=h[0]; ++i)
      {
        for (int j=l[1]; j<=h[1]; ++j)
          output << i << " " << j << " " << m(i,j) << std::endl;
        output << std::endl;
      }
    }
    break;
    case 3:
    {
      DataGrid3d &m = *(meshData.get3dMesh(0));
      GridIndex3d l = m.getLo(), h = m.getHi();

      for (int i=l[0]; i<=h[0]; ++i)
      {
        for (int j=l[1]; j<=h[1]; ++j)
          for (int k=l[2]; k<=h[2]; ++k)
          output << i << " " << j << " " << k << " " << m(i,j,k) << std::endl;
        output << std::endl;
      }
    }
    break;
  }

  output.close();
}

void McfdCommand_tohdf::print_help()
{
  std::cout << "\n  Manipulate cfd files: convert cfd block to HDF5 format\n\n  Usage:\n"
        << "    mcfd toh5 [options] <block> <input> [<output>]\n\n"
        << "  where <block> is the name of the data block in the cfd file,\n"
        << "  <input> is the name of the cfd file,\n"
        << "  and <output> is the name of the hdf file to write.\n\n";

  std::cout << option_desc;
}
