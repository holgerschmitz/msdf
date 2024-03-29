/*
 * mcfd.cpp
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#include "commands.hpp"
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <iostream>
#include "common/binaryio.hpp"

using namespace msdf;

CommandMap msdfCommands;

int main(int argc, char **argv)
{
  register_commands(msdfCommands);

  if (argc < 2)
  {
    print_help(msdfCommands);
    exit(-1);
  }

  std::string command(argv[1]);

  if (msdfCommands.count(command)==0)
  {
    print_help(msdfCommands);
    exit(-1);
  }

  pMsdfCommand theCommand = msdfCommands[command]->makeCommand();
  try
  {
    theCommand->execute(argc-1, &(argv[1]));
  }
  catch (BlockNotFoundException &ex)
  {
    std::cerr << "ERROR!\n" << "Block '"<< ex.getName() << "' not found\n";
  }
  catch (BlockTypeUnsupportedException &ex)
  {
    std::cerr << "ERROR!\n" << "Block type '"<< ex.getBlockType()
        << "' unsupported in block '"<< ex.getBlockName() <<"'\n";
  }
  catch (GenericException &ex)
  {
    std::cerr << "ERROR!\n" << ex.getMessage() <<"\n";
  }

}
