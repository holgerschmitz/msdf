/*
 * cfdblock.h
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef CFDBLOCK_H_
#define CFDBLOCK_H_

#include "cfdio.hpp"
#include "cfdheader.hpp"
#include <string>
#include <list>

class CfdBlockHeader : public CfdIo
{
  private:
      pCfdFileHeader fileHeader;

      std::string name;
      std::string blockClass;
      BlockType blockType;
      int64_t lengthBlockMetadata;
      int64_t lengthBlock;

      int64_t blockOffset;
      int64_t blockOffsetHeader;

  public:
    CfdBlockHeader(CfdFile &file); //Header fileHeader_, pIstream cfdStream);
    CfdBlockHeader(const CfdBlockHeader &header);
    void skipBlock(pIstream cfdStream);
    pCfdBlockData getData(CfdFile &file);
    pCfdBlockDataStream getDataStream(CfdFile &file);

    pCfdFileHeader getFileHeader() const
    {
        return fileHeader;
    }

    std::string getBlockClass() const
    {
        return blockClass;
    }

    int64_t getBlockOffset() const
    {
        return blockOffset;
    }

    int64_t getBlockOffsetHeader() const
    {
        return blockOffsetHeader;
    }

    BlockType getBlockType() const
    {
        return blockType;
    }

    std::string getBlockTypeStr() const
    {
        return BlockTypeToString(blockType);
    }

    int64_t getLengthBlock() const
    {
        return lengthBlock;
    }

    int64_t getLengthBlockMetadata() const
    {
        return lengthBlockMetadata;
    }

    std::string getName() const
    {
        return name;
    }

  private:

    void readHeader(pIstream cfdStream);
};

typedef boost::shared_ptr<CfdBlockHeader> pCfdBlockHeader;

typedef std::list<pCfdBlockHeader> CfdBlockHeaderList;
typedef boost::shared_ptr<CfdBlockHeaderList> pCfdBlockHeaderList;

#endif /* CFDBLOCK_H_ */
