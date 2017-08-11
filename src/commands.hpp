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

/** @brief Interface class for all msdf commands
 *
 * Each msdf command must implement the execute() function that will execute the command,
 * and the print_help() function that should use prind out usage information for the command.
 * Specifically, boost::program_options should be used to create a summary of command options.
 */
class MsdfCommand
{
  public:
    /// Virtual destructor is needed
    virtual ~MsdfCommand() {}

    /** @brief Abstract method that must be implemented to perform the command
     *
     * @param argc The number of command line arguments, excluding the msdf command itself
     * @param argv The command line arguments, excluding the msdf command itself
     */
    virtual void execute(int argc, char **argv) = 0;
    /** @brief Abstract method that must be implemented to provide usage information for the command.
     *
     * This should print out a description of the command and what it does together with a complete
     * summary of all command line options.
     */
    virtual void print_help() = 0;
};

/// A shared pointer to a MsdfCommand object
typedef boost::shared_ptr<MsdfCommand> pMsdfCommand;


class McfdCommandFactory
{
  public:
    virtual ~McfdCommandFactory() {}
    virtual std::string name() = 0;
    virtual std::string description() = 0;
    virtual pMsdfCommand makeCommand() = 0;
};

typedef boost::shared_ptr<McfdCommandFactory> pMcfdCommandFactory;

typedef std::map<std::string, pMcfdCommandFactory> CommandMap;

void register_commands(CommandMap &map);
void print_help(CommandMap &map);

//===========================================================
//====================    ls command    =====================
//===========================================================

class McfdCommandInfo_ls : public McfdCommandFactory
{
  public:
    std::string name() { return "ls"; }

    std::string description()
    {
      return "list the contents of an SDF file";
    }

    pMsdfCommand makeCommand();
};

//===========================================================
//====================    tohdf command    ==================
//===========================================================

class McfdCommandInfo_tohdf : public McfdCommandFactory
{
  public:
    std::string name() { return "toh5"; }

    std::string description()
    {
      return "writes a data block to HDF5";
    }

    pMsdfCommand makeCommand();
};


//===========================================================
//====================    pcount command    ==================
//===========================================================

class McfdCommandInfo_pcount : public McfdCommandFactory
{
  public:
    std::string name() { return "pcount"; }

    std::string description()
    {
      return "counts the number of particles of each species";
    }

    pMsdfCommand makeCommand();
};


//===========================================================
//====================    penergy command    ==================
//===========================================================

class McfdCommandInfo_penergy : public McfdCommandFactory
{
  public:
    std::string name() { return "penergy"; }

    std::string description()
    {
      return "calculates (nonrelativistic!) thermal energies of particles of each species";
    }

    pMsdfCommand makeCommand();
};


//===========================================================
//====================    phaseplot command    ==================
//===========================================================

class McfdCommandInfo_phaseplot : public McfdCommandFactory
{
  public:
    std::string name() { return "phaseplot"; }

    std::string description()
    {
      return "Writes phase-plots for species and stores in hdf5 format.";
    }

    pMsdfCommand makeCommand();
};


//===========================================================
//====================    screen command    ==================
//===========================================================

class McfdCommandInfo_screen : public McfdCommandFactory
{
  public:
    std::string name() { return "screen"; }

    std::string description()
    {
      return "Projects particles onto a screen and writes in ascii format.";
    }

    pMsdfCommand makeCommand();
};


//===========================================================
//====================    angular command    ==================
//===========================================================

class McfdCommandInfo_angular : public McfdCommandFactory
{
  public:
    std::string name() { return "angular"; }

    std::string description()
    {
      return "Writes angular distribution plot for each species and stores in plain ascii format files readable by GNUplot.";
    }

    pMsdfCommand makeCommand();
};

//===========================================================
//====================    distfunc command    ==================
//===========================================================

class McfdCommandInfo_distfunc : public McfdCommandFactory
{
  public:
    std::string name() { return "distfunc"; }

    std::string description()
    {
      return "Writes integrated distribution function for each species and stores in plain ascii format files readable by GNUplot.";
    }

    pMsdfCommand makeCommand();
};

#endif /* COMMANDS_H_ */
