/*
 * sdffile.cpp
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "sdffile.hpp"
#include <fstream>

namespace msdf {

  SdfFile::SdfFile(pIstream sdfStream_)
    : sdfStream(sdfStream_),
      header(new SdfFileHeader(sdfStream_)),
      blockHeaderList(new SdfBlockHeaderList)
  {
    readBlockHeaderList();
  }

  SdfFile::SdfFile(std::string & fileName)
    : blockHeaderList(new SdfBlockHeaderList)
  {
    sdfStream = pIstream( new std::fstream(fileName.c_str()) );
    header = pSdfFileHeader(new SdfFileHeader(sdfStream));
    readBlockHeaderList();
  }

  void SdfFile::rewind()
  {
    blockPointer = header->getFirstBlockLocation();
    sdfStream->seekg(blockPointer);
  }

  void SdfFile::readBlockHeaderList()
  {
    blockHeaderList->clear();
    this->rewind();

    for (int i=0; i<header->getNumBlocks(); ++i)
    {
      pSdfBlockHeader blockHead(new SdfBlockHeader(*this));
      blockHeaderList->push_back(blockHead);
      blockHead->skipBlock(sdfStream);
    }
  }

  pSdfBlockHeader SdfFile::getBlockHeader(std::string name)
  {
    SdfBlockHeaderList::iterator it;

    for (it = blockHeaderList->begin(); it != blockHeaderList->end(); ++it)
    {
      pSdfBlockHeader bl = *it;
      if (name == bl->getName()) return bl;
    }
    throw(BlockNotFoundException(name));
  }
}
