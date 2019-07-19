/*
 * commands.h
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef MSDF_COMMANDS_H_
#define MSDF_COMMANDS_H_

#include <boost/shared_ptr.hpp>

#include <string>
#include <map>

namespace msdf {

  /**
   * @brief Interface class for all msdf commands
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

      /**
       * @brief Abstract method that must be implemented to perform the command
       *
       * @param argc The number of command line arguments, excluding the msdf command itself
       * @param argv The command line arguments, excluding the msdf command itself
       */
      virtual void execute(int argc, char **argv) = 0;

      /**
       * @brief Abstract method that must be implemented to provide usage information for the command.
       *
       * This should print out a description of the command and what it does together with a complete
       * summary of all command line options.
       */
      virtual void print_help() = 0;
  };

  /// A shared pointer to a MsdfCommand object
  typedef boost::shared_ptr<MsdfCommand> pMsdfCommand;

  /**
   * @brief A factory for creating SDF commands
   *
   * SDF commands are passed as the first argument to `msdf`. The factory is used
   * to create a command on demand.
   *
   * @see MsdfCommand
   */
  class MsdfCommandFactory
  {
    public:
      /// Virtual destructor is needed
      virtual ~MsdfCommandFactory() {}

      /**
       * @brief The name of the command
       *
       * This is the command name as supplied by the user and is used to look up
       * the factory.
       *
       * @return the name of the command
       */
      virtual std::string name() = 0;

      /**
       * @brief A short description of the command.
       *
       * This is used to generate the help text
       *
       * @return the description of the command
       */
      virtual std::string description() = 0;

      /**
       * Create the command
       *
       * @return a new instance of the command
       */
      virtual pMsdfCommand makeCommand() = 0;
  };

  /**
   * A shared pointer to the command factory
   */
  typedef boost::shared_ptr<MsdfCommandFactory> pMsdfCommandFactory;

  /**
   * A map of command indexed by their name
   */
  typedef std::map<std::string, pMsdfCommandFactory> CommandMap;

  /**
   * @brief Register all known commands in the map
   *
   * Currently the following commands are registered:
   *
   * * McfdCommandInfo_ls (McfdCommand_ls)
   * * McfdCommandInfo_tohdf (McfdCommand_tohdf)
   * * McfdCommandInfo_pcount (McfdCommand_pcount)
   * * McfdCommandInfo_penergy (McfdCommand_penergy)
   * * McfdCommandInfo_phaseplot (McfdCommand_phaseplot)
   * * McfdCommandInfo_screen (McfdCommand_screen)
   * * McfdCommandInfo_angular (McfdCommand_angular)
   * * McfdCommandInfo_distfunc (McfdCommand_distfunc)
   */
  void register_commands(CommandMap &map);

  /**
   * Prints a help text
   */
  void print_help(CommandMap &map);

  //===========================================================
  //====================    ls command    =====================
  //===========================================================

  /**
   * Command factory for the `ls` command
   */
  class McfdCommandInfo_ls : public MsdfCommandFactory
  {
    public:

      std::string name() { return "ls"; }

      std::string description()
      {
        return "list the contents of an SDF file";
      }

      /**
       * Create the `ls` command
       *
       * @return a new instance of McfdCommand_ls
       */
      pMsdfCommand makeCommand();
  };

  //===========================================================
  //====================    tohdf command    ==================
  //===========================================================

  /**
   * Command factory for the `toh5` command
   */
  class McfdCommandInfo_tohdf : public MsdfCommandFactory
  {
    public:
      std::string name() { return "toh5"; }

      std::string description()
      {
        return "writes a data block to HDF5";
      }

      /**
       * Create the `toh5` command
       *
       * @return a new instance of McfdCommand_tohdf
       */
      pMsdfCommand makeCommand();
  };


  //===========================================================
  //====================    pcount command    ==================
  //===========================================================

  /**
   * Command factory for the `pcount` command
   */
  class McfdCommandInfo_pcount : public MsdfCommandFactory
  {
    public:
      std::string name() { return "pcount"; }

      std::string description()
      {
        return "counts the number of particles of each species";
      }

      /**
       * Create the `pcount` command
       *
       * @return a new instance of McfdCommand_pcount
       */
      pMsdfCommand makeCommand();
  };


  //===========================================================
  //====================    penergy command    ==================
  //===========================================================

  /**
   * Command factory for the `penergy` command
   */
  class McfdCommandInfo_penergy : public MsdfCommandFactory
  {
    public:
      std::string name() { return "penergy"; }

      std::string description()
      {
        return "calculates (nonrelativistic!) thermal energies of particles of each species";
      }

      /**
       * Create the `penergy` command
       *
       * @return a new instance of McfdCommand_penergy
       */
      pMsdfCommand makeCommand();
  };


  //===========================================================
  //====================    phaseplot command    ==================
  //===========================================================

  /**
   * Command factory for the `phaseplot` command
   */
  class McfdCommandInfo_phaseplot : public MsdfCommandFactory
  {
    public:
      std::string name() { return "phaseplot"; }

      std::string description()
      {
        return "Writes phase-plots for species and stores in hdf5 format.";
      }

      /**
       * Create the `phaseplot` command
       *
       * @return a new instance of McfdCommand_phaseplot
       */
      pMsdfCommand makeCommand();
  };


  //===========================================================
  //====================    screen command    ==================
  //===========================================================

  /**
   * Command factory for the `screen` command
   */
  class McfdCommandInfo_screen : public MsdfCommandFactory
  {
    public:
      std::string name() { return "screen"; }

      std::string description()
      {
        return "Projects particles onto a screen and writes in ascii format.";
      }

      /**
       * Create the `screen` command
       *
       * @return a new instance of McfdCommand_screen
       */
      pMsdfCommand makeCommand();
  };


  //===========================================================
  //====================    angular command    ==================
  //===========================================================

  /**
   * Command factory for the `angular` command
   */
  class McfdCommandInfo_angular : public MsdfCommandFactory
  {
    public:
      std::string name() { return "angular"; }

      std::string description()
      {
        return "Writes angular distribution plot for each species and stores in plain ascii format files readable by GNUplot.";
      }

      /**
       * Create the `angular` command
       *
       * @return a new instance of MsdfCommand_angular
       */
      pMsdfCommand makeCommand();
  };

  //===========================================================
  //====================    distfunc command    ==================
  //===========================================================

  /**
   * Command factory for the `distfunc` command
   */
  class McfdCommandInfo_distfunc : public MsdfCommandFactory
  {
    public:
      std::string name() { return "distfunc"; }

      std::string description()
      {
        return "Writes integrated distribution function for each species and stores in plain ascii format files readable by GNUplot.";
      }

      /**
       * Create the `distfunc` command
       *
       * @return a new instance of McfdCommand_distfunc
       */
      pMsdfCommand makeCommand();
  };

} // namespace msdf

#endif /* MSDF_COMMANDS_H_ */
