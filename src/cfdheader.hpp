/*
 * cfdheader.h
 *
 *  Created on: 4 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef CFDHEADER_H_
#define CFDHEADER_H_

#include "cfdio.hpp"

class CfdFile;

class CfdFileHeader : public CfdIo
{
  public:
    CfdFileHeader(pIstream cfdStream);

    int32_t getLengthFileHeader() {return length_file_header; }
    int32_t getLengthBlockHeader() {return length_block_header; }
    int32_t getVersionMajor() {return version_major; }
    int32_t getVersionMinor() {return version_minor; }
    int32_t getStringLength() {return string_length; }
    int32_t getNumBlocks() {return num_blocks; }

  private:

    int32_t length_file_header;
    int32_t length_block_header;
    int32_t version_major;
    int32_t version_minor;
    int32_t string_length;
    int32_t num_blocks;

    void readHeader(pIstream cfdStream);
};

typedef boost::shared_ptr<CfdFileHeader> pCfdFileHeader;

#endif /* CFDHEADER_H_ */
