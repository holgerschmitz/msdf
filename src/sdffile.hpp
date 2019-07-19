/*
 * sdffile.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef MSDF_SDFFILE_H_
#define MSDF_SDFFILE_H_

#include "common/binaryio.hpp"
#include "sdfheader.hpp"
#include "sdfblock.hpp"

namespace msdf {

  /**
   * @brief Information about an SDF file
   */
  class SdfFile {
    private:
      /**
       * Read the block headers
       */
      void readBlockHeaderList();

      /**
       * Reset the file pointer to the first block header
       */
      void rewind();

      /**
       * The input stream from which the SDF file is read
       */
      pIstream sdfStream;

      /**
       * The file header information
       */
      pSdfFileHeader header;

      /**
       * The complete lost of blocks in the file
       */
      pSdfBlockHeaderList blockHeaderList;

      /**
       * The current block index
       */
      int blockPointer;

    public:
      /**
       * Construct with an input stream
       *
       * @param sdfStream the stream to read from
       */
      SdfFile(pIstream sdfStream);

      /**
       * Construct from file
       *
       * Creates a `std::fstream` from the file name
       *
       * @param fileName  the file name
       */
      SdfFile(std::string &fileName);

      /**
       * Get the file header
       */
      pSdfFileHeader getHeader() const
      {
          return header;
      }

      /**
       * Get the list of block headers
       */
      pSdfBlockHeaderList getBlockHeaderList()
      {
          return blockHeaderList;
      }

//      pIstream getStream() const { return sdfStream; }

      /**
       * Get the block header with a specific name
       */
      pSdfBlockHeader getBlockHeader(std::string name);
  };

  /**
   * A shared pointer to an SdfFile
   */
  typedef boost::shared_ptr<SdfFile> pSdfFile;

} // namespace msdf

#endif /* SDFFILE_H_ */
