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

void store_command_in_map(CommandMap &map, McfdCommandInfo *info)
{
  map[info->name()] = pMcfdCommandInfo(info);
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
  std::cout << "\n  Manipulate cfd files\n\n  Usage:\n"
      << "    mcfd command [options]\n\n"
      << "  where 'command' is one of the following:\n\n";

  for (CommandMap::iterator it = map.begin(); it != map.end(); ++it)
  {
    pMcfdCommandInfo info(it->second);
    std::cout << "      "<< std::setiosflags(std::ios::left) << std::setw(10)
        << info->name() << "  " << info->description() << std::endl;
  }

  std::cout << "\n  Type\n    mcfd help command\n  to get more help on individual commands.\n\n";
}



pMcfdCommand McfdCommandInfo_ls::makeCommand()
{
  return pMcfdCommand(new McfdCommand_ls());
}

pMcfdCommand McfdCommandInfo_tohdf::makeCommand()
{
  return pMcfdCommand(new McfdCommand_tohdf());
}

pMcfdCommand McfdCommandInfo_pcount::makeCommand()
{
  return pMcfdCommand(new McfdCommand_pcount());
}

pMcfdCommand McfdCommandInfo_penergy::makeCommand()
{
  return pMcfdCommand(new McfdCommand_penergy());
}

pMcfdCommand McfdCommandInfo_phaseplot::makeCommand()
{
  return pMcfdCommand(new McfdCommand_phaseplot());
}

pMcfdCommand McfdCommandInfo_screen::makeCommand()
{
  return pMcfdCommand(new McfdCommand_screen());
}

pMcfdCommand McfdCommandInfo_angular::makeCommand()
{
  return pMcfdCommand(new McfdCommand_angular());
}

pMcfdCommand McfdCommandInfo_distfunc::makeCommand()
{
  return pMcfdCommand(new McfdCommand_distfunc());
}
