/*
 * cfdblock.cpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "cfdblock.hpp"
#include "cfdfile.hpp"
#include "cfddatatypes.hpp"

#include <iostream>

CfdBlockHeader::CfdBlockHeader(CfdFile &file)
  : fileHeader(file.getHeader())
{
  readHeader(file.getStream());
}

CfdBlockHeader::CfdBlockHeader(const CfdBlockHeader &header)
    : fileHeader(header.fileHeader),
      name(header.name),
      blockClass(header.blockClass),
      blockType(header.blockType),
      lengthBlockMetadata(header.lengthBlockMetadata),
      lengthBlock(header.lengthBlock),
      blockOffset(header.blockOffset),
      blockOffsetHeader(header.blockOffsetHeader)
{ }

void CfdBlockHeader::readHeader(pIstream cfdStream)
{
  int stringLength = fileHeader->getStringLength();

  blockOffsetHeader = cfdStream->tellg();

  this->readString(*cfdStream, name, stringLength);
  this->readString(*cfdStream, blockClass, stringLength);
  int32_t bType;
  this->readValue(*cfdStream, bType);
  blockType = intToBlockType(bType);

  this->readValue(*cfdStream, lengthBlockMetadata);
  this->readValue(*cfdStream, lengthBlock);

  blockOffset = cfdStream->tellg();
}

void CfdBlockHeader::skipBlock(pIstream cfdStream)
{
  cfdStream->seekg(blockOffset + lengthBlock);
}

pCfdBlockData CfdBlockHeader::getData(CfdFile &file)
{
  pIstream cfdStream = file.getStream();
  cfdStream->seekg(blockOffset);

  switch (blockType)
  {
//    case mesh:
//      return this->getMesh(cfdStream);
    case mesh_variable:
      return pCfdBlockData(new CfdMeshVariable(cfdStream, file.getHeader(), *this));
//    case snapshot:
//      return this->getSnapshot(cfdStream);
    default:
      throw(BlockTypeUnsupportedException(name, BlockTypeToString(blockType)));
  }
}


pCfdBlockDataStream CfdBlockHeader::getDataStream(CfdFile &file)
{
  pIstream cfdStream = file.getStream();
  cfdStream->seekg(blockOffset);

  switch (blockType)
  {
//    case mesh:
//      return this->getMesh(cfdStream);
    case mesh_variable:
      return pCfdBlockDataStream(new CfdMeshVariableStream(cfdStream, file.getHeader(), *this, 1024*1024));
//    case snapshot:
//      return this->getSnapshot(cfdStream);
    default:
      throw(BlockTypeUnsupportedException(name, BlockTypeToString(blockType)));
  }
}

