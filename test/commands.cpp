/*
 * commands.cpp
 *
 *  Created on: 19 Jul 2019
 *      Author: Holger Schmitz
 */

#include <commands.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE( commands )

BOOST_AUTO_TEST_CASE( register_ls )
{
  const std::string cmd = "ls";
  CommandMap commands;
  register_commands(commands);

  BOOST_CHECK_EQUAL(commands.count(cmd), 1);
  pMsdfCommandFactory factory = commands[cmd];

  BOOST_CHECK_NE(boost::dynamic_pointer_cast<McfdCommandInfo_ls>(factory), NULL);

  pMsdfCommand commandInstance = factory->makeCommand();

  BOOST_CHECK_NE(boost::dynamic_pointer_cast<McfdCommand_ls>(commandInstance), NULL);
}


BOOST_AUTO_TEST_SUITE_END()

