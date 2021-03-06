/*
 * sdfheader.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFHEADER_H_
#define SDFHEADER_H_

#include "sdfio.hpp"

namespace msdf {

  class SdfFile;

  /**
   * The SDF file header information
   */
  class SdfFileHeader
  {
    public:
      /**
       * Create the file header from an input stream
       */
      SdfFileHeader(pIstream sdfStream);

      /**
       * Get the position of the first block
       */
      int32_t getFirstBlockLocation() {return first_block_location; }

      /**
       * Get the length of the block header
       */
      int32_t getLengthBlockHeader() {return block_header_length; }

      /**
       * Get the major SDF version of the file
       */
      int32_t getVersionMajor() {return sdf_version; }

      /**
       * Get the minor SDF version of the file
       */
      int32_t getVersionMinor() {return sdf_revision; }

      /**
       * Get the length of a string
       */
      int32_t getStringLength() {return string_length; }

      /**
       * Get the number of blocks in the file
       */
      int32_t getNumBlocks() {return num_blocks; }

    private:
      int32_t endianness;
      int32_t sdf_version;
      int32_t sdf_revision;
      std::string code_name;
      int64_t first_block_location;
      int64_t summary_location;
      int32_t summary_size;
      int32_t num_blocks;
      int32_t block_header_length;
      int32_t step;
      double time;
      int32_t jobid1;
      int32_t jobid2;
      int32_t string_length;
      int32_t code_io_version;
      char restart_flag;
      char subdomain_file;

      //int32_t
      //int32_t length_file_header;
      //int32_t length_block_header;
      //int32_t version_major;
      //int32_t version_minor;
      //int32_t string_length;
      //int32_t num_blocks;

      /**
       * Read the header information from the file
       */
      void readHeader(pIstream cfdStream);
  };

  /**
   * A shared pointer to SdfFileHeader
   */
  typedef boost::shared_ptr<SdfFileHeader> pSdfFileHeader;

} // namespace msdf

#endif /* SDFHEADER_H_ */
