/*
 * sdfheader.cpp
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "sdfheader.hpp"
#include <iostream>

SdfFileHeader::SdfFileHeader(pIstream sdfStream)
{
  readHeader(sdfStream);
}

void SdfFileHeader::readHeader(pIstream sdfStream)
{
  char marker[4];
  sdfStream->read(marker,4);
  this->readValue(*sdfStream, endianness);
  this->readValue(*sdfStream, sdf_version);
  this->readValue(*sdfStream, sdf_revision);
  this->readString(*sdfStream, code_name, 32);
  this->readValue(*sdfStream, first_block_location);
  this->readValue(*sdfStream, summary_location);
  this->readValue(*sdfStream, summary_size);
  this->readValue(*sdfStream, num_blocks);
  this->readValue(*sdfStream, block_header_length);
  this->readValue(*sdfStream, step);
  this->readValue(*sdfStream, time);
  this->readValue(*sdfStream, jobid1);
  this->readValue(*sdfStream, jobid2);
  this->readValue(*sdfStream, string_length);
  this->readValue(*sdfStream, code_io_version);
  this->readValue(*sdfStream, restart_flag);
  this->readValue(*sdfStream, subdomain_file);

//  std::cerr << "first_block_location = " << first_block_location << "\n" <<
//               "summary_location = " << summary_location << "\n" <<
//               "summary_size = " << summary_size << "\n" <<

}