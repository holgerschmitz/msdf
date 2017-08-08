/*
 * cfdfile.cpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 */

#include "cfdfile.hpp"
#include <fstream>

CfdFile::CfdFile(pIstream cfdStream_)
  : cfdStream(cfdStream_),
    header(new CfdFileHeader(cfdStream_)),
    blockHeaderList(new CfdBlockHeaderList)
{
  this->rewind();
  readBlockHeaderList();
}

CfdFile::CfdFile(std::string & fileName)
  : blockHeaderList(new CfdBlockHeaderList)
{
  cfdStream = pIstream( new std::fstream(fileName.c_str()) );
  header = pCfdFileHeader(new CfdFileHeader(cfdStream));
  this->rewind();
  readBlockHeaderList();
}

void CfdFile::rewind()
{
  blockPointer = header->getLengthFileHeader();
  cfdStream->seekg(blockPointer);
}

void CfdFile::readBlockHeaderList()
{
  blockHeaderList->clear();
  this->rewind();

  for (int i=0; i<header->getNumBlocks(); ++i)
  {
    pCfdBlockHeader blockHead(new CfdBlockHeader(*this));
    blockHeaderList->push_back(blockHead);
    blockHead->skipBlock(cfdStream);
  }
}

pCfdBlockHeader CfdFile::getBlockHeader(std::string name)
{
  CfdBlockHeaderList::iterator it;

  for (it = blockHeaderList->begin(); it != blockHeaderList->end(); ++it)
  {
    pCfdBlockHeader bl = *it;
    if (name == bl->getName()) return bl;
  }

  throw(BlockNotFoundException(name));
}
