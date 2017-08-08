/*
 * commands.h
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <boost/shared_ptr.hpp>

#include <string>
#include <map>

class McfdCommand
{
  public:
    virtual void execute(int argc, char **argv) = 0;
    virtual void print_help() = 0;
};

typedef boost::shared_ptr<McfdCommand> pMcfdCommand;


class McfdCommandInfo
{
  public:
    virtual std::string name() = 0;
    virtual std::string description() = 0;
    virtual pMcfdCommand makeCommand() = 0;
};

typedef boost::shared_ptr<McfdCommandInfo> pMcfdCommandInfo;

typedef std::map<std::string, pMcfdCommandInfo> CommandMap;

void register_commands(CommandMap &map);
void print_help(CommandMap &map);

//===========================================================
//====================    ls command    =====================
//===========================================================

class McfdCommandInfo_ls : public McfdCommandInfo
{
  public:
    std::string name() { return "ls"; }

    std::string description()
    {
      return "list the contents of a cfd file";
    }

    pMcfdCommand makeCommand();
};

//===========================================================
//====================    tohdf command    ==================
//===========================================================

class McfdCommandInfo_tohdf : public McfdCommandInfo
{
  public:
    std::string name() { return "toh5"; }

    std::string description()
    {
      return "writes a data block to HDF5";
    }

    pMcfdCommand makeCommand();
};


//===========================================================
//====================    pcount command    ==================
//===========================================================

class McfdCommandInfo_pcount : public McfdCommandInfo
{
  public:
    std::string name() { return "pcount"; }

    std::string description()
    {
      return "counts the number of particles of each species";
    }

    pMcfdCommand makeCommand();
};


//===========================================================
//====================    penergy command    ==================
//===========================================================

class McfdCommandInfo_penergy : public McfdCommandInfo
{
  public:
    std::string name() { return "penergy"; }

    std::string description()
    {
      return "calculates (nonrelativistic!) thermal energies of particles of each species";
    }

    pMcfdCommand makeCommand();
};


//===========================================================
//====================    phaseplot command    ==================
//===========================================================

class McfdCommandInfo_phaseplot : public McfdCommandInfo
{
  public:
    std::string name() { return "phaseplot"; }

    std::string description()
    {
      return "Writes phase-plots for species and stores in hdf5 format.";
    }

    pMcfdCommand makeCommand();
};


//===========================================================
//====================    screen command    ==================
//===========================================================

class McfdCommandInfo_screen : public McfdCommandInfo
{
  public:
    std::string name() { return "screen"; }

    std::string description()
    {
      return "Projects particles onto a screen and writes in ascii format.";
    }

    pMcfdCommand makeCommand();
};


//===========================================================
//====================    angular command    ==================
//===========================================================

class McfdCommandInfo_angular : public McfdCommandInfo
{
  public:
    std::string name() { return "angular"; }

    std::string description()
    {
      return "Writes angular distribution plot for each species and stores in plain ascii format files readable by GNUplot.";
    }

    pMcfdCommand makeCommand();
};

//===========================================================
//====================    distfunc command    ==================
//===========================================================

class McfdCommandInfo_distfunc : public McfdCommandInfo
{
  public:
    std::string name() { return "distfunc"; }

    std::string description()
    {
      return "Writes integrated distribution function for each species and stores in plain ascii format files readable by GNUplot.";
    }

    pMcfdCommand makeCommand();
};

#endif /* COMMANDS_H_ */
