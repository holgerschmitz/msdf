/*
 * sdfheader.cpp
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "sdfheader.hpp"
#include <iostream>

namespace msdf {

SdfFileHeader::SdfFileHeader(pIstream sdfStream)
{
  readHeader(sdfStream);
}

void SdfFileHeader::readHeader(pIstream sdfStream)
{
  char marker[4];
  sdfStream->read(marker,4);
  detail::readValue(*sdfStream, endianness);
  detail::readValue(*sdfStream, sdf_version);
  detail::readValue(*sdfStream, sdf_revision);
  detail::readString(*sdfStream, code_name, 32);
  detail::readValue(*sdfStream, first_block_location);
  detail::readValue(*sdfStream, summary_location);
  detail::readValue(*sdfStream, summary_size);
  detail::readValue(*sdfStream, num_blocks);
  detail::readValue(*sdfStream, block_header_length);
  detail::readValue(*sdfStream, step);
  detail::readValue(*sdfStream, time);
  detail::readValue(*sdfStream, jobid1);
  detail::readValue(*sdfStream, jobid2);
  detail::readValue(*sdfStream, string_length);
  detail::readValue(*sdfStream, code_io_version);
  detail::readValue(*sdfStream, restart_flag);
  detail::readValue(*sdfStream, subdomain_file);

//  std::cerr << "first_block_location = " << first_block_location << "\n" <<
//               "summary_location = " << summary_location << "\n" <<
//               "summary_size = " << summary_size << "\n" <<//               "num_blocks = " << num_blocks << "\n" <<//               "block_header_length = " << block_header_length << "\n" <<//               "step = " << step << "\n" <<//               "time = " << time << "\n" <<//               "jobid1 = " << jobid1 << "\n" <<//               "jobid2 = " << jobid2 << "\n" <<//               "string_length = " << string_length << "\n" <<//               "code_io_version = " << code_io_version << "\n" <<//               "restart_flag = " << restart_flag << "\n" <<//               "subdomain_file = " << subdomain_file << "\n";

}

} // namespace msdf
