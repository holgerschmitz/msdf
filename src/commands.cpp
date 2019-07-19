/*
 * commands.cpp
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#include "commands.hpp"

#include "ls.hpp"
#include "tohdf.hpp"
#include "pcount.hpp"
#include "penergy.hpp"
#include "phaseplot.hpp"
#include "screen.hpp"
#include "angular.hpp"
#include "distfunc.hpp"


#include <iostream>
#include <iomanip>

using namespace msdf;

void store_command_in_map(CommandMap &map, MsfdCommandFactory *info)
{
  map[info->name()] = pMsfdCommandFactory(info);
}

void register_commands(CommandMap &map)
{
  store_command_in_map(map, new McfdCommandInfo_ls);
  store_command_in_map(map, new McfdCommandInfo_tohdf);
  store_command_in_map(map, new McfdCommandInfo_pcount);
  store_command_in_map(map, new McfdCommandInfo_penergy);
  store_command_in_map(map, new McfdCommandInfo_phaseplot);
  store_command_in_map(map, new McfdCommandInfo_screen);
  store_command_in_map(map, new McfdCommandInfo_angular);
  store_command_in_map(map, new McfdCommandInfo_distfunc);
}

void print_help(CommandMap &map)
{
  std::cout << "\n  Manipulate SDF files\n\n  Usage:\n"
      << "    msdf command [options]\n\n"
      << "  where 'command' is one of the following:\n\n";

  for (CommandMap::iterator it = map.begin(); it != map.end(); ++it)
  {
    pMsfdCommandFactory info(it->second);
    std::cout << "      "<< std::setiosflags(std::ios::left) << std::setw(10)
        << info->name() << "  " << info->description() << std::endl;
  }

  std::cout << "\n  Type\n    mcfd help command\n  to get more help on individual commands.\n\n";
}



pMsdfCommand McfdCommandInfo_ls::makeCommand()
{
  return pMsdfCommand(new McfdCommand_ls());
}

pMsdfCommand McfdCommandInfo_tohdf::makeCommand()
{
  return pMsdfCommand(new McfdCommand_tohdf());
}

pMsdfCommand McfdCommandInfo_pcount::makeCommand()
{
  return pMsdfCommand(new McfdCommand_pcount());
}

pMsdfCommand McfdCommandInfo_penergy::makeCommand()
{
  return pMsdfCommand(new McfdCommand_penergy());
}

pMsdfCommand McfdCommandInfo_phaseplot::makeCommand()
{
  return pMsdfCommand(new McfdCommand_phaseplot());
}

pMsdfCommand McfdCommandInfo_screen::makeCommand()
{
  return pMsdfCommand(new McfdCommand_screen());
}

pMsdfCommand McfdCommandInfo_angular::makeCommand()
{
  return pMsdfCommand(new MsdfCommand_angular());
}

pMsdfCommand McfdCommandInfo_distfunc::makeCommand()
{
  return pMsdfCommand(new McfdCommand_distfunc());
}
