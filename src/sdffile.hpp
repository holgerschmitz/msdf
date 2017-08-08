/*
 * sdffile.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFFILE_H_
#define SDFFILE_H_

#include "sdfheader.hpp"
#include "sdfblock.hpp"

class SdfFile {
  private:
    void readBlockHeaderList();
    void rewind();

    pIstream sdfStream;
    pSdfFileHeader header;
    pSdfBlockHeaderList blockHeaderList;
    int blockPointer;

  public:
    SdfFile(pIstream cfdStream_);
    SdfFile(std::string &fileName);

    pSdfFileHeader getHeader() const
    {
        return header;
    }

    pSdfBlockHeaderList getBlockHeaderList()
    {
        return blockHeaderList;
    }

    pIstream getStream() const { return sdfStream; }

    pSdfBlockHeader getBlockHeader(std::string name);


};

typedef boost::shared_ptr<SdfFile> pSdfFile;

#endif /* SDFFILE_H_ */
