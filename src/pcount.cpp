/*
 * pcount.cpp
 *
 *  Created on: 19 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "pcount.hpp"
#include "particlestream.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

namespace po = boost::program_options;

McfdCommand_pcount::McfdCommand_pcount()
  : option_desc("Options for the 'pcount' command")
{
  streamFact.addSpecies().setProgramOptions(option_desc);

  option_pos.add("input", 1);
}


void McfdCommand_pcount::execute(int argc, char **argv)
{
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  pParticleStream pstream = streamFact.getParticleStream(vm);

  if (!pstream)
  {
    print_help();
    exit(-1);
  }

  int maxId = 0;
  int smallId = 0;
  long maxPos = 0;
  long pos = 0;
  std::vector<int> speciesCounts;

  pstream->getNextChunks();
  while (! pstream->eos() )
  {
    for (
        DataGrid1d::const_storage_iterator it = pstream->species->cbegin();
        it != pstream->species->cend();
        ++it, ++pos
        )
    {
      int id = *it;
      if (id > maxId)
      {
        speciesCounts.resize(id,0); // I know the 0 is redundant, but I'm paranoid!
        maxId = id;
      }
      if (id < 1) ++smallId;
      else {
        ++speciesCounts[id-1];
        maxPos = pos;
      }
    }
    pstream->getNextChunks();
  }
  std::cout << "Maximum valid particle position " << maxPos << std::endl;

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

  std::cout  << std::setiosflags(std::ios::left) << "  Specied Id    Count\n";
  for (unsigned int id = 0; id<speciesCounts.size(); ++id)
  {
    std::cout << std::setiosflags(std::ios::right) << "  "
        << std::setw(6) << id+1  << "     "
        << std::setw(10) << speciesCounts[id]  << "\n";
  }
}


void McfdCommand_pcount::print_help()
{
  std::cout << "\n  Manipulate cfd files: counts the number of particles of each species\n\n  Usage:\n"
        << "    mcfd pcount [options] <input>\n\n"
        << "  where <input> is the name of the cfd file.\n\n";

  std::cout << option_desc;
}
