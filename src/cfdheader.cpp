/*
 * cfdheader.cpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 */

#include "cfdheader.hpp"
#include <iostream>

CfdFileHeader::CfdFileHeader(pIstream cfdStream)
{
  readHeader(cfdStream);
}

void CfdFileHeader::readHeader(pIstream cfdStream)
{
  char marker[3];
  cfdStream->read(marker,3);
  this->readValue(*cfdStream, length_file_header);
  this->readValue(*cfdStream, length_block_header);
  this->readValue(*cfdStream, version_major);
  this->readValue(*cfdStream, version_minor);
  this->readValue(*cfdStream, string_length);
  this->readValue(*cfdStream, num_blocks);

//  std::cerr << "length_file_header = " << length_file_header << "\n"
//      << "length_block_header = " << length_block_header << "\n"
//      << "string_length = " << string_length << "\n"
//      << "num_blocks = " << num_blocks << "\n";
}
