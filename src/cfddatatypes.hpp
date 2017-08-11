/*
 * cfddatatypes.h
 *
 *  Created on: 6 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef CFDDATATYPES_H_
#define CFDDATATYPES_H_

#include "cfdio.hpp"
#include "cfdheader.hpp"
#include "cfdblock.hpp"
#include "msdf.hpp"

class CfdMeshVariable : public CfdBlockData
{
  public:
    CfdMeshVariable(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block);
    BlockType getBlockType() { return mesh_variable; }
    int getRank();
    pDataGrid1d get1dMesh();
    pDataGrid2d get2dMesh();
    pDataGrid3d get3dMesh();

    double getMin();
    double getMax();
  private:
    int32_t rank;
    MeshType meshType;
    int32_t precision;

    pDataGrid1d mesh1d;
    pDataGrid2d mesh2d;
    pDataGrid3d mesh3d;

    IntArray arrsize;
    DataGrid1d stagger;
    DataGrid1d extents;

    std::string meshName;
    std::string meshClass;

    void readData(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block);

    template<typename realtype>
    void readDataByPrecision(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype);

    template<typename realtype, class GridType>
    void readMeshData(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype, GridType &grid);

    template<typename realtype>
    void readParticleDataByPrecision(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype);

    template<typename realtype, class GridType>
    void readParticleData(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype, GridType &grid);
};

class CfdMeshVariableStream : public CfdBlockDataStream
{
  public:
    CfdMeshVariableStream(pIstream cfdStream_, pCfdFileHeader header, const CfdBlockHeader &block_, int64_t chunkLength_);
    BlockType getBlockType() { return mesh_variable; }
    void getMeshChunk(pDataGrid1d chunk);
    bool eos() { return activeCount>dataLength; }
  private:
    pIstream cfdStream;
    pCfdFileHeader header;
    CfdBlockHeader block;

    int32_t rank;
    MeshType meshType;
    int32_t precision;
    int64_t chunkLength;

    int64_t dataLength;
    DataGrid1d extents;

    std::string meshName;
    std::string meshClass;


    int64_t activeOffset;
    int64_t activeCount;

    void initStream();

    template<typename realtype>
    void initStreamByPrecision(realtype);

    template<typename realtype>
    void readChunkByPrecision(int64_t chsize, pDataGrid1d chunk, realtype);
};

typedef boost::shared_ptr<CfdMeshVariableStream> pCfdMeshVariableStream;

class CfdMeshStream : public CfdBlockDataStream
{
  public:
    CfdMeshStream(pIstream cfdStream_, pCfdFileHeader header, const CfdBlockHeader &block_, int64_t chunkLength_);
    BlockType getBlockType() { return mesh_variable; }
    void getMeshChunk(pDataGrid2d chunk);
    bool eos() { return activeCount>dataLength; }
  private:
    pIstream cfdStream;
    pCfdFileHeader header;
    CfdBlockHeader block;

    int32_t rank;
    MeshType meshType;
    int32_t precision;
    int64_t chunkLength;

    int64_t dataLength;
    DataGrid1d extents;

    std::string meshName;
    std::string meshClass;


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

typedef boost::shared_ptr<CfdMeshStream> pCfdMeshStream;



#endif /* CFDDATATYPES_H_ */
