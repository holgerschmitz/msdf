/*
 * binaryio.hpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: holger@notjustphysics.com
 */

#ifndef BINARYIO_H_
#define BINARYIO_H_

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <iostream>
#include <cassert>

namespace msdf {

  /// @cond HIDDEN_SYMBOLS
  namespace detail {
    typedef boost::shared_ptr<std::istream> pIstream;

    /**
     * Read a typed value from a binary stream
     *
     * @tparam T  the type of data to read
     * @param in  the input stream to read from
     * @param data  the variable that will receive the value to be read
     */
    template<typename T>
    inline void readValue(std::istream &in, T &data)
    {
      void *ptr = &data;
      char *ch = (char*)ptr;

      in.read(ch,sizeof(T));
    }

    /**
     * Read a string value from a binary stream. Any whitespace will be trimmed
     *
     * @param in   the input stream to read from
     * @param str  the string that will receive the value to be read
     * @param length  the number of characters to read
     */
    inline void readString(std::istream &in, std::string &str, size_t length)
    {
      assert(length<1024);
      char buffer[1024];
      in.read(buffer,length);
      str = std::string(buffer);
      boost::trim(str);
    }
  }
  /// @endcond

  /**
   * An exception that is thrown when a block has not been found
   */
  class BlockNotFoundException
  {
    private:
      /**
       * The name of the block
       */
      std::string name;
    public:
      /**
       * Constructor that takes the name of the data block
       *
       * @param name_  the name of the block
       */
      BlockNotFoundException(std::string name_) throw()
          : name(name_) {}

      /**
       * Copy constructor
       *
       * @param ex  the exception to copy from
       */
      BlockNotFoundException(const BlockNotFoundException &ex) throw()
          : name(ex.name) {}

      /**
       * Assignment operator
       *
       * @param ex  the exception to copy from
       */
      BlockNotFoundException &operator=(const BlockNotFoundException &ex) throw()
      { name = ex.name; return *this; }

      /**
       * Get the name of the block that could not be found
       *
       * @return  the name of the block that caused the exception
       */
      std::string getName() const throw()
      { return name;  }
  };

  class BlockTypeUnsupportedException
  {
    private:
      std::string blockname;
      std::string blocktype;
    public:
      BlockTypeUnsupportedException(std::string blockname_, std::string blocktype_) throw()
          : blockname(blockname_), blocktype(blocktype_) {}

      BlockTypeUnsupportedException(const BlockTypeUnsupportedException &ex) throw()
          : blockname(ex.blockname), blocktype(ex.blocktype) {}

      BlockTypeUnsupportedException &operator=(const BlockTypeUnsupportedException &ex) throw()
      { blockname=ex.blockname; blocktype=ex.blocktype;  return *this; }

      std::string getBlockName() const throw()
      { return blockname;  }

      std::string getBlockType() const throw()
      { return blocktype;  }
  };


  class DataTypeUnsupportedException
  {
    private:
      std::string blockname;
      std::string datatype;
    public:
      DataTypeUnsupportedException(std::string blockname_, std::string datatype_) throw()
          : blockname(blockname_), datatype(datatype_) {}

      DataTypeUnsupportedException(const DataTypeUnsupportedException &ex) throw()
          : blockname(ex.blockname), datatype(ex.datatype) {}

      DataTypeUnsupportedException &operator=(const DataTypeUnsupportedException &ex) throw()
      { blockname=ex.blockname; datatype=ex.datatype;  return *this; }

      std::string getBlockName() const throw()
      { return blockname;  }

      std::string getDataType() const throw()
      { return datatype;  }
  };

  class GenericException
  {
    private:
      std::string message;
    public:
      GenericException(std::string message_) throw()
          : message(message_) {}

      GenericException(const GenericException &ex) throw()
          : message(ex.message) {}

      GenericException &operator=(const GenericException &ex) throw()
      { message = ex.message; return *this; }

      std::string getMessage() const throw()
      { return message;  }
  };
}

#endif /* BINARYIO_H_ */
