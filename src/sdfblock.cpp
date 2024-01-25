/*
 * sdfblock.cpp
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "sdfblock.hpp"
#include "common/sdffile.hpp"
#include "sdfdatatypes.hpp"

#include <iostream>

SdfBlockHeader::SdfBlockHeader(SdfFile &file)
  : fileHeader(file.getHeader())
{
  readHeader(file.getStream());
}

SdfBlockHeader::SdfBlockHeader(const SdfBlockHeader &header)
    : fileHeader(header.fileHeader),
      name(header.name),
      id(header.id),
      next_block_location(header.next_block_location),
      data_location(header.data_location),
      data_length(header.data_length),
      blockType(header.blockType),
      data_type(header.data_type),
      ndims(header.ndims),
      blockOffsetHeader(header.blockOffsetHeader)
{ }

void SdfBlockHeader::readHeader(pIstream sdfStream)
{
  int stringLength = fileHeader->getStringLength();

  blockOffsetHeader = sdfStream->tellg();

  msdf::detail::readValue(*sdfStream, next_block_location);
  msdf::detail::readValue(*sdfStream, data_location);
  msdf::detail::readString(*sdfStream, id, 32);
  msdf::detail::readValue(*sdfStream, data_length);

  int32_t bType;
  msdf::detail::readValue(*sdfStream, bType);
  blockType = intToBlockType(bType);

  msdf::detail::readValue(*sdfStream, bType);
  data_type = intToDataType(bType);

  msdf::detail::readValue(*sdfStream, ndims);


  msdf::detail::readString(*sdfStream, name, stringLength);
}

void SdfBlockHeader::skipBlock(pIstream sdfStream)
{
  sdfStream->seekg(next_block_location);
}

pSdfBlockData SdfBlockHeader::getData(SdfFile &file)
{
  pIstream sdfStream = file.getStream();
//  sdfStream->seekg(data_location);

  switch (blockType)
  {
//    case mesh:
//      return this->getMesh(cfdStream);
    case sdf_plain_variable:
    case sdf_point_variable:
      return pSdfBlockData(new SdfMeshVariable(sdfStream, file.getHeader(), *this));
//    case snapshot:
//      return this->getSnapshot(cfdStream);
    default:
      throw(BlockTypeUnsupportedException(name, BlockTypeToString(blockType)));
  }
}


pSdfBlockDataStream SdfBlockHeader::getDataStream(SdfFile &file)
{
  pIstream sdfStream = file.getStream();
//  sdfStream->seekg(data_location);

  switch (blockType)
  {
//    case mesh:
//      return this->getMesh(cfdStream);
    case sdf_point_variable:
      return pSdfBlockDataStream(new SdfMeshVariableStream(sdfStream, file.getHeader(), *this, 1024*1024));
//    case snapshot:
//      return this->getSnapshot(cfdStream);
    default:
      throw(BlockTypeUnsupportedException(name, BlockTypeToString(blockType)));
  }
}

