/*
 * sdfblock.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFBLOCK_H_
#define SDFBLOCK_H_

#include "sdfio.hpp"
#include "sdfheader.hpp"
#include <string>
#include <list>



class SdfBlockHeader : public SdfIo
{
  private:
      pSdfFileHeader fileHeader;

      std::string name;
      std::string id;
      int64_t next_block_location;
      int64_t data_location;
      int64_t data_length;
      SdfBlockType blockType;
      SdfDataType data_type;
      int32_t ndims;

      int64_t blockOffsetHeader;
  public:
    SdfBlockHeader(SdfFile &file); //Header fileHeader_, pIstream cfdStream);
    SdfBlockHeader(const SdfBlockHeader &header);
    void skipBlock(pIstream cfdStream);
    pSdfBlockData getData(SdfFile &file);
    pSdfBlockDataStream getDataStream(SdfFile &file);

    pSdfFileHeader getFileHeader() const
    {
        return fileHeader;
    }

    int64_t getDataLocation() const
    {
        return data_location;
    }

    int64_t getBlockOffsetHeader() const
    {
        return blockOffsetHeader;
    }

    int64_t getMetaDataOffset() const
    {
        return blockOffsetHeader + fileHeader->getLengthBlockHeader();
    }

    SdfBlockType getBlockType() const
    {
        return blockType;
    }

    std::string getBlockTypeStr() const
    {
        return BlockTypeToString(blockType);
    }

    SdfDataType getDataType() const
    {
        return data_type;
    }

    std::string getDataTypeStr() const
    {
        return DataTypeToString(data_type);
    }

    int32_t getNDims() const
    {
        return ndims;
    }

    std::string getName() const
    {
        return id;
    }

    std::string getScreenName() const
    {
        return name;
    }

  private:

    void readHeader(pIstream cfdStream);
};

typedef boost::shared_ptr<SdfBlockHeader> pSdfBlockHeader;

typedef std::list<pSdfBlockHeader> SdfBlockHeaderList;
typedef boost::shared_ptr<SdfBlockHeaderList> pSdfBlockHeaderList;

#endif /* SDFBLOCK_H_ */
