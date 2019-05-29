/*
 * binaryio_spec.cpp
 *
 *  Created on: 29 May 2019
 *      Author: Holger Schmitz
 */

#include <common/binaryio.hpp>

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE( range )

BOOST_AUTO_TEST_CASE( readValue_int )
{
  std::stringstream buffer;

  int num = 1234567890;
  buffer.write((char const*) &num, sizeof(num));

  int dest;
  msdf::detail::readValue(buffer, dest);
  BOOST_CHECK_EQUAL(dest, num);
}

BOOST_AUTO_TEST_CASE( readValue_double )
{
  std::stringstream buffer;

  double num = 3.14159;
  buffer.write((char const*) &num, sizeof(num));

  double dest;
  msdf::detail::readValue(buffer, dest);
  BOOST_CHECK_EQUAL(dest, num);
}

BOOST_AUTO_TEST_CASE( readString )
{
  std::string str = "foo-bar-baz-qux";
  std::istringstream buffer(str);

  std::string dest;
  msdf::detail::readString(buffer, dest, str.size());
  BOOST_CHECK_EQUAL(dest, str);
}

BOOST_AUTO_TEST_CASE( readString_clipped )
{
  std::string str = "foo-bar-baz-qux";
  std::istringstream buffer(str);

  std::string dest;
  msdf::detail::readString(buffer, dest, 3);
  BOOST_CHECK_EQUAL(dest, "foo");
}

BOOST_AUTO_TEST_CASE( block_not_found_exception_constructor )
{
  const msdf::BlockNotFoundException ex("foo");
  BOOST_CHECK_EQUAL(ex.getName(), "foo");

  const msdf::BlockNotFoundException ex2(ex);
  BOOST_CHECK_EQUAL(ex2.getName(), "foo");
}

BOOST_AUTO_TEST_CASE( block_not_found_exception_assign)
{
  const msdf::BlockNotFoundException ex("foo");
  msdf::BlockNotFoundException ex2("bar");

  ex2 = ex;
  BOOST_CHECK_EQUAL(ex2.getName(), "foo");
}

BOOST_AUTO_TEST_CASE( block_type_unsupported_exception_constructor )
{
  const msdf::BlockTypeUnsupportedException ex("foo", "bar");
  BOOST_CHECK_EQUAL(ex.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex.getBlockType(), "bar");

  const msdf::BlockTypeUnsupportedException ex2(ex);
  BOOST_CHECK_EQUAL(ex2.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex2.getBlockType(), "bar");
}

BOOST_AUTO_TEST_CASE( block_type_unsupported_exception_assign)
{
  const msdf::BlockTypeUnsupportedException ex("foo", "bar");
  msdf::BlockTypeUnsupportedException ex2("baz", "qux");

  ex2 = ex;
  BOOST_CHECK_EQUAL(ex2.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex2.getBlockType(), "bar");
}

BOOST_AUTO_TEST_CASE( data_type_unsupported_exception_constructor )
{
  const msdf::DataTypeUnsupportedException ex("foo", "bar");
  BOOST_CHECK_EQUAL(ex.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex.getDataType(), "bar");

  const msdf::DataTypeUnsupportedException ex2(ex);
  BOOST_CHECK_EQUAL(ex2.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex2.getDataType(), "bar");
}

BOOST_AUTO_TEST_CASE( data_type_unsupported_exception_assign)
{
  const msdf::DataTypeUnsupportedException ex("foo", "bar");
  msdf::DataTypeUnsupportedException ex2("baz", "qux");

  ex2 = ex;
  BOOST_CHECK_EQUAL(ex2.getBlockName(), "foo");
  BOOST_CHECK_EQUAL(ex2.getDataType(), "bar");
}

BOOST_AUTO_TEST_CASE( generic_exception_constructor )
{
  const msdf::GenericException ex("foo");
  BOOST_CHECK_EQUAL(ex.getMessage(), "foo");

  const msdf::GenericException ex2(ex);
  BOOST_CHECK_EQUAL(ex2.getMessage(), "foo");
}

BOOST_AUTO_TEST_CASE( generic_exception_assign)
{
  const msdf::GenericException ex("foo");
  msdf::GenericException ex2("bar");

  ex2 = ex;
  BOOST_CHECK_EQUAL(ex2.getMessage(), "foo");
}


BOOST_AUTO_TEST_SUITE_END()
