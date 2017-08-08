/*
 * cfdfile.h
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef CFDFILE_H_
#define CFDFILE_H_

#include "cfdheader.hpp"
#include "cfdblock.hpp"

class CfdFile {
  private:
    void readBlockHeaderList();
    void rewind();

    pIstream cfdStream;
    pCfdFileHeader header;
    pCfdBlockHeaderList blockHeaderList;
    int blockPointer;

  public:
    CfdFile(pIstream cfdStream_);
    CfdFile(std::string &fileName);

    pCfdFileHeader getHeader() const
    {
        return header;
    }

    pCfdBlockHeaderList getBlockHeaderList()
    {
        return blockHeaderList;
    }

    pIstream getStream() const { return cfdStream; }

    pCfdBlockHeader getBlockHeader(std::string name);


};

typedef boost::shared_ptr<CfdFile> pCfdFile;

#endif /* CFDFILE_H_ */
