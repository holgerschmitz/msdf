/*
 * binaryio.hpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: holger@notjustphysics.com
 */

#ifndef MSDF_BINARYIO_H_
#define MSDF_BINARYIO_H_

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <iostream>
#include <cassert>

namespace msdf {

  typedef boost::shared_ptr<std::istream> pIstream;

  /// @cond HIDDEN_SYMBOLS
  namespace detail {

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
      char buffer[1025];
      in.read(buffer,length);
      buffer[length] = 0;
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

  /**
   * An exception that is thrown when a block type is not supported
   */
  class BlockTypeUnsupportedException
  {
    private:

      /**
       * The name of the block
       */
      std::string blockname;

      /**
       * The type of the block
       */
      std::string blocktype;
    public:

      /**
       * Constructor that takes the name and type of the data block
       *
       * @param blockname_  the name of the block
       * @param blocktype_  the type of the block
       */
      BlockTypeUnsupportedException(std::string blockname_, std::string blocktype_) throw()
          : blockname(blockname_), blocktype(blocktype_) {}

      /**
       * Copy constructor
       *
       * @param ex  the exception to copy from
       */
      BlockTypeUnsupportedException(const BlockTypeUnsupportedException &ex) throw()
          : blockname(ex.blockname), blocktype(ex.blocktype) {}

      /**
       * Assignment operator
       *
       * @param ex  the exception to copy from
       */
      BlockTypeUnsupportedException &operator=(const BlockTypeUnsupportedException &ex) throw()
      { blockname=ex.blockname; blocktype=ex.blocktype;  return *this; }

      /**
       * Get the name of the block that caused the exception
       *
       * @return  the name of the block
       */
      std::string getBlockName() const throw()
      { return blockname;  }

      /**
       * Get the type of the block that caused the exception
       *
       * @return  the type of the block
       */
      std::string getBlockType() const throw()
      { return blocktype;  }
  };


  /**
   * An exception that is thrown when a data type is not supported
   */
  class DataTypeUnsupportedException
  {
    private:

      /**
       * The name of the block
       */
      std::string blockname;

      /**
       * The data type of the block
       */
      std::string datatype;
    public:

      /**
       * Constructor that takes the name and data type of the block
       *
       * @param blockname_  the name of the block
       * @param datatype_  the data type of the block
       */
      DataTypeUnsupportedException(std::string blockname_, std::string datatype_) throw()
          : blockname(blockname_), datatype(datatype_) {}

      /**
       * Copy constructor
       *
       * @param ex  the exception to copy from
       */
      DataTypeUnsupportedException(const DataTypeUnsupportedException &ex) throw()
          : blockname(ex.blockname), datatype(ex.datatype) {}

      /**
       * Assignment operator
       *
       * @param ex  the exception to copy from
       */
      DataTypeUnsupportedException &operator=(const DataTypeUnsupportedException &ex) throw()
      { blockname=ex.blockname; datatype=ex.datatype;  return *this; }

      /**
       * Get the name of the block that caused the exception
       *
       * @return  the name of the block
       */
      std::string getBlockName() const throw()
      { return blockname;  }

      /**
       * Get the data type of the block that caused the exception
       *
       * @return  the data type of the block
       */
      std::string getDataType() const throw()
      { return datatype;  }
  };

  /**
   * A generic exception
   */
  class GenericException
  {
    private:

      /**
       * The message
       */
      std::string message;
    public:

      /**
       * Constructor that takes the message
       *
       * @param message_  the human readable message
       */
      GenericException(std::string message_) throw()
          : message(message_) {}

      /**
       * Copy constructor
       *
       * @param ex  the exception to copy from
       */
      GenericException(const GenericException &ex) throw()
          : message(ex.message) {}

      /**
       * Assignment operator
       *
       * @param ex  the exception to copy from
       */
      GenericException &operator=(const GenericException &ex) throw()
      { message = ex.message; return *this; }

      /**
       * Get the exception message
       *
       * @return  the message
       */
      std::string getMessage() const throw()
      { return message;  }
  };

} // namespace msdf

#endif /* MSDF_BINARYIO_H_ */
