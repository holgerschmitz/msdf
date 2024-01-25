/*
 * ls.cpp
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#include "ls.hpp"
#include "common/sdffile.hpp"
#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace po = boost::program_options;


McfdCommand_ls::McfdCommand_ls()
  : option_desc("Options for the 'ls' command")
{
  option_desc.add_options()
      ("file,f", po::value<std::string>(&fileName), "name of the cfd file");

  option_pos.add("file", 1);
}

void McfdCommand_ls::execute(int argc, char **argv)
{
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  if (vm.count("file")<1)
  {
    print_help();
    exit(-1);
  }

  pIstream iStream( new std::fstream(fileName.c_str()) );

  SdfFile file(iStream);

  pSdfBlockHeaderList blocks = file.getBlockHeaderList();
  SdfBlockHeaderList::iterator it;

  int blockCount = 1;
  unsigned int maxNameLength = 0;
  for (it = blocks->begin(); it != blocks->end(); ++it)
  {
    pSdfBlockHeader bl = *it;
    if  (bl->getName().length()>maxNameLength) maxNameLength=bl->getName().length();
  }

  std::cout  << std::setiosflags(std::ios::left) << "  #    "
      << std::setw(maxNameLength+1) << std::string("Name")
      << "Offset       Type\n";


  for (it = blocks->begin(); it != blocks->end(); ++it)
  {
    pSdfBlockHeader bl = *it;
    std::cout << std::setiosflags(std::ios::left) << "  "
      << std::setw(5) << blockCount++
      << std::setw(maxNameLength+2) << bl->getName()
      << std::setw(15) << bl->getBlockOffsetHeader()
      << bl->getBlockTypeStr() << std::endl;
  }
}

void McfdCommand_ls::print_help()
{
  std::cout << "\n  Manipulate cfd files: list cfd file contents\n\n  Usage:\n"
        << "    mcfd ls [options] <filename>\n\n"
        << "  where <filename> is the name of the cfd file.\n\n";

  std::cout << option_desc;
}
