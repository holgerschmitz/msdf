/*
 * sdfdatatypes.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFDATATYPES_H_
#define SDFDATATYPES_H_

#include "sdfio.hpp"
#include "mcfd.hpp"

#include "sdfheader.hpp"
#include "sdfblock.hpp"

class SdfMeshVariable : public SdfBlockData
{
  private:
    SdfBlockHeader block;
  public:
    SdfMeshVariable(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block_);
    SdfBlockType getBlockType() { return block.getBlockType(); }
    int getRank();
    pDataGrid1d get1dMesh();
    pDataGrid2d get2dMesh();
    pDataGrid3d get3dMesh();

    double getMin();
    double getMax();
  private:
    int32_t rank;
    int32_t precision;

    double mult;
    std::string units;
    std::string meshId;

    pDataGrid1d mesh1d;
    pDataGrid2d mesh2d;
    pDataGrid3d mesh3d;

    IntArray arrsize;

    DataGrid1d stagger;
    DataGrid1d extents;


    void readData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block);

    template<typename realtype>
    void readDataByPrecision(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype);

    template<typename realtype, class GridType>
    void readMeshData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype, GridType &grid);

    template<typename realtype>
    void readParticleDataByPrecision(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype);

    template<typename realtype, class GridType>
    void readParticleData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype, GridType &grid);
};

class SdfMeshVariableStream : public SdfBlockDataStream
{
  private:
    SdfBlockHeader block;
  public:
    SdfMeshVariableStream(pIstream sdfStream_, pSdfFileHeader header, const SdfBlockHeader &block_, int64_t chunkLength_);
    SdfBlockType getBlockType() { return block.getBlockType(); }
    void getMeshChunk(pDataGrid1d chunk);
    bool eos() { return activeCount>dataLength; }
  private:
    pIstream sdfStream;
    pSdfFileHeader header;
    int64_t chunkLength;

    int32_t rank;
    int32_t precision;

    double mult;
    std::string units;
    std::string meshId;

    int64_t dataLength;

    int64_t activeOffset;
    int64_t activeCount;

    void initStream();

    template<typename realtype>
    void initStreamByPrecision(realtype);

    template<typename realtype>
    void readChunkByPrecision(int64_t chsize, pDataGrid1d chunk, realtype);
};

typedef boost::shared_ptr<SdfMeshVariableStream> pSdfMeshVariableStream;

class SdfMeshStream : public SdfBlockDataStream
{
  private:
    SdfBlockHeader block;
  public:
    SdfMeshStream(pIstream sdfStream_, pSdfFileHeader header, const SdfBlockHeader &block_, int64_t chunkLength_);
    SdfBlockType getBlockType() { return block.getBlockType(); }
    void getMeshChunk(pDataGrid2d chunk);
    bool eos() { return activeCount>dataLength; }
  private:
    pIstream sdfStream;
    pSdfFileHeader header;
    int64_t chunkLength;

    int32_t rank;
    int32_t precision;
    int32_t geometry;

    std::vector<double> mults;
    std::vector<std::string> units;
    std::vector<std::string> labels;
    std::vector<double> minvals;
    std::vector<double> maxvals;

    int64_t dataLength;

    int64_t activeOffset;
    int64_t activeCount;

    void initStream();

    template<typename realtype>
    void initStreamByPrecision(realtype);

    template<typename realtype>
    void readChunkByPrecision(int64_t chsize, pDataGrid2d chunk, realtype);
  public:

    int getRank() { return rank; }
};

typedef boost::shared_ptr<SdfMeshStream> pSdfMeshStream;



#endif /* SDFDATATYPES_H_ */
