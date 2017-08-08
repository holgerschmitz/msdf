/*
 * cfddatatypes.cpp
 *
 *  Created on: 6 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */
#include "cfddatatypes.hpp"

#include <schnek/typetools.hpp>
#include <schnek/vector.hpp>
#include <algorithm>

//==============================================================================
//==========================  CfdMechVariable  =================================
//==============================================================================


CfdMeshVariable::CfdMeshVariable(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block)
{
  this->readData(cfdStream, header, block);
}

pDataGrid1d CfdMeshVariable::get1dMesh()
{
  if (rank!=1) throw GenericException("Wrong mesh dimension!");
  return mesh1d;
}

pDataGrid2d CfdMeshVariable::get2dMesh()
{
  if (rank!=2) throw GenericException("Wrong mesh dimension!");
  return mesh2d;
}

pDataGrid3d CfdMeshVariable::get3dMesh()
{
  if (rank!=3) throw GenericException("Wrong mesh dimension!");
  return mesh3d;
}

int CfdMeshVariable::getRank()
{
  return rank;
}

double CfdMeshVariable::getMin()
{
  switch (getRank())
  {
    case 1:
      return *std::min_element(this->mesh1d->begin(), this->mesh1d->end());
      break;
    case 2:
      return *std::min_element(this->mesh2d->begin(), this->mesh2d->end());
      break;
    case 3:
      return *std::min_element(this->mesh3d->begin(), this->mesh3d->end());
      break;
  }
}

double CfdMeshVariable::getMax()
{
  switch (getRank())
  {
    case 1:
      return *std::max_element(this->mesh1d->begin(), this->mesh1d->end());
      break;
    case 2:
      return *std::max_element(this->mesh2d->begin(), this->mesh2d->end());
      break;
    case 3:
      return *std::max_element(this->mesh3d->begin(), this->mesh3d->end());
      break;
  }
}

void CfdMeshVariable::readData(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block)
{
  int32_t meshTypeInt;
  readValue(*cfdStream,meshTypeInt);
  meshType = intToMeshType(meshTypeInt);
  readValue(*cfdStream,rank);
  readValue(*cfdStream,precision);

  if (meshType == other_mesh) throw GenericException("Data block contains unsupported mesh type!");

  if (precision==sizeof(float)) {
    std::cerr << "  reading single precision\n";
    switch (meshType)
    {
      case cartesian_mesh:
        readDataByPrecision(cfdStream, header, block, schnek::Type2Type<float>());
        break;
      case particle_mesh:
        readParticleDataByPrecision(cfdStream, header, block, schnek::Type2Type<float>());
        break;
      default:
        break;
    }
  }
  else if (precision==sizeof(double)) {
    std::cerr << "  reading double precision\n";
    switch (meshType)
    {
      case cartesian_mesh:
        readDataByPrecision(cfdStream, header, block, schnek::Type2Type<double>());
        break;
      case particle_mesh:
        readParticleDataByPrecision(cfdStream, header, block, schnek::Type2Type<float>());
        break;
      default:
        break;
    }
  }
  else throw GenericException("CFD file contains unsupported precision data!");
}


template<typename realtype>
void CfdMeshVariable::readDataByPrecision(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype rt)
{

  std::cerr << "  rank is " << rank << "\n";
  switch (rank)
  {
    case 1:
      mesh1d = pDataGrid1d(new DataGrid1d());
      this->readMeshData(cfdStream, header, block, rt, *mesh1d);
      break;
    case 2:
      mesh2d = pDataGrid2d(new DataGrid2d());
      this->readMeshData(cfdStream, header, block, rt, *mesh2d);
      break;
    case 3:
      mesh3d = pDataGrid3d(new DataGrid3d());
      this->readMeshData(cfdStream, header, block, rt, *mesh3d);
      break;
    default:
      throw GenericException("CFD file contains mesh data with rank other than 1,2 or 3!");
      break;
  }
}


template<typename realtype, class GridType>
void CfdMeshVariable::readMeshData(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype, GridType &grid)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<int32_t, GridType::Rank> Index;
  typedef schnek::Vector<Real, GridType::Rank> Vector;
  typedef schnek::Vector<Real, 2> Bounds;

  const int rank = GridType::Rank;

  Index arrsize;
  Vector stagger;
  Bounds extents;

  for (int i=0; i<rank; ++i) this->readValue(*cfdStream, arrsize[rank-1-i]);
  for (int i=0; i<rank; ++i) this->readValue(*cfdStream, stagger[i]);
  for (int i=0; i<2; ++i)    this->readValue(*cfdStream,extents[i]);

  this->readString(*cfdStream, meshName, stringLength);
  this->readString(*cfdStream, meshClass, stringLength);

  cfdStream->seekg(block.getBlockOffset() + block.getLengthBlockMetadata());

  grid.resize(arrsize);

  // we don't use arrsize.product() because the return type is only int32_t and
  // might be too small
  int64_t length = 1;
  for (int i=0; i<rank; ++i) length *= arrsize[i];

  Real *data;
  if (sizeof(Real)==sizeof(typename GridType::value_type))
    data = (Real*)grid.getRawData();
  else
    data = new Real[length];

  // for reading from a character stream
  char *ch = (char*)data;

  cfdStream->read(ch, length*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename GridType::value_type))
  {
    typename GridType::value_type *raw = grid.getRawData();
    for (int64_t i=0; i<length; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }

}

template<typename realtype>
void CfdMeshVariable::readParticleDataByPrecision(pIstream cfdStream, pCfdFileHeader header, CfdBlockHeader &block, realtype rt)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<Real, 2> Bounds;

  Bounds extents;

  int64_t npart;
  this->readValue(*cfdStream, npart);
  for (int i=0; i<2; ++i)    this->readValue(*cfdStream,extents[i]);

  this->readString(*cfdStream, meshName, stringLength);
  this->readString(*cfdStream, meshClass, stringLength);

  cfdStream->seekg(block.getBlockOffset() + block.getLengthBlockMetadata());

  mesh1d->resize(GridIndex1d(npart));

  Real *data;
  if (sizeof(Real)==sizeof(typename DataGrid1d::value_type))
    data = (Real*)(mesh1d->getRawData());
  else
    data = new Real[npart];

  // for reading from a character stream
  char *ch = (char*)data;

  cfdStream->read(ch, npart*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename DataGrid1d::value_type))
  {
    typename DataGrid1d::value_type *raw = mesh1d->getRawData();
    for (int64_t i=0; i<npart; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }
}

//==============================================================================
//==========================  CfdMeshVariableStream  ===========================
//==============================================================================

CfdMeshVariableStream::CfdMeshVariableStream(pIstream cfdStream_, pCfdFileHeader header_, const CfdBlockHeader &block_, int64_t chunkLength_)
    : cfdStream(cfdStream_),
      header(header_),
      block(block_),
      chunkLength(chunkLength_)
{
  this->initStream();
}

void CfdMeshVariableStream::initStream()
{
  cfdStream->seekg(block.getBlockOffset());
  int32_t meshTypeInt;
  readValue(*cfdStream,meshTypeInt);
  meshType = intToMeshType(meshTypeInt);
  readValue(*cfdStream,rank);
  readValue(*cfdStream,precision);

  if (meshType != particle_mesh) throw GenericException("Data block contains unsupported mesh type!");

  if (precision==sizeof(float))
  {
    initStreamByPrecision(schnek::Type2Type<float>());
  }
  else if (precision==sizeof(double))
  {
    initStreamByPrecision(schnek::Type2Type<double>());
  }
}

template<typename realtype>
void CfdMeshVariableStream::initStreamByPrecision(realtype)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<Real, 2> Bounds;

  Bounds extents;

  this->readValue(*cfdStream, dataLength);
  for (int i=0; i<2; ++i)    this->readValue(*cfdStream,extents[i]);

  this->readString(*cfdStream, meshName, stringLength);
  this->readString(*cfdStream, meshClass, stringLength);

  activeOffset = block.getBlockOffset() + block.getLengthBlockMetadata();
  activeCount = 0;

//  std::cerr << "activeOffset = " << activeOffset << "\n";
//  std::cerr << "activeCount = " << activeCount << "\n";
}

void CfdMeshVariableStream::getMeshChunk(pDataGrid1d chunk)
{
  int64_t chsize = chunkLength;
  if (dataLength-activeCount < chsize) chsize = dataLength-activeCount;

  GridIndex1d gridSize = chunk->getDims();
  if (gridSize[0] != chsize)
  {
    gridSize[0] = chsize;
    chunk->resize(gridSize);
  }

  if (chsize>0)
  {
    cfdStream->seekg(activeOffset);
    if (precision==sizeof(float))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<float>());
    }
    else if (precision==sizeof(double))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<double>());
    }
    activeCount += chsize;
    activeOffset = cfdStream->tellg();
  }
  else
  {
    activeCount = dataLength+1;
  }
}


template<typename realtype>
void CfdMeshVariableStream::readChunkByPrecision(int64_t chsize, pDataGrid1d chunk, realtype)
{
  typedef typename realtype::OriginalType Real;
  Real *data;
  if (sizeof(Real)==sizeof(typename DataGrid1d::value_type))
    data = (Real*)(chunk->getRawData());
  else
    data = new Real[chsize];

  // for reading from a character stream
  char *ch = (char*)data;

  cfdStream->read(ch, chsize*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename DataGrid1d::value_type))
  {
    typename DataGrid1d::value_type *raw = chunk->getRawData();
    for (int64_t i=0; i<chsize; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }
}


//==============================================================================
//==========================  CfdMeshStream  ===========================
//==============================================================================

CfdMeshStream::CfdMeshStream(pIstream cfdStream_, pCfdFileHeader header_, const CfdBlockHeader &block_, int64_t chunkLength_)
    : cfdStream(cfdStream_),
      header(header_),
      block(block_),
      chunkLength(chunkLength_)
{
  this->initStream();
}

void CfdMeshStream::initStream()
{
  cfdStream->seekg(block.getBlockOffset());
  int32_t meshTypeInt;
  readValue(*cfdStream,meshTypeInt);
  meshType = intToMeshType(meshTypeInt);
  readValue(*cfdStream,rank);
  readValue(*cfdStream,precision);

  if (meshType != particle_mesh) throw GenericException("Data block contains unsupported mesh type!");

  if (precision==sizeof(float))
  {
    initStreamByPrecision(schnek::Type2Type<float>());
  }
  else if (precision==sizeof(double))
  {
    initStreamByPrecision(schnek::Type2Type<double>());
  }
}

template<typename realtype>
void CfdMeshStream::initStreamByPrecision(realtype)
{
  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<Real, 2> Bounds;

  Bounds extents;

  this->readValue(*cfdStream, dataLength);
  for (int i=0; i<2; ++i) this->readValue(*cfdStream,extents[i]);

  activeOffset = block.getBlockOffset() + block.getLengthBlockMetadata();
  activeCount = 0;
}

void CfdMeshStream::getMeshChunk(pDataGrid2d chunk)
{
  int64_t chsize = chunkLength;
  if (dataLength-activeCount < chsize) chsize = dataLength-activeCount;

  GridIndex2d gridSize;
  gridSize[0] = chunk->getDims()[0];
  gridSize[1] = chunk->getDims()[1];

  if ((gridSize[0] != rank) || (gridSize[1] != chsize))
  {
    gridSize[0] = rank;
    gridSize[1] = chsize;
    chunk->resize(gridSize);
  }

  if (chsize>0)
  {
    cfdStream->seekg(activeOffset);
    if (precision==sizeof(float))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<float>());
    }
    else if (precision==sizeof(double))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<double>());
    }
    activeCount += chsize;
    activeOffset = cfdStream->tellg();
  }
  else
  {
    activeCount = dataLength+1;
  }
}


template<typename realtype>
void CfdMeshStream::readChunkByPrecision(int64_t chsize, pDataGrid2d chunk, realtype)
{
  typedef typename realtype::OriginalType Real;
  Real *data;
  int64_t dataSize = rank*chsize;
  if (sizeof(Real)==sizeof(typename DataGrid1d::value_type))
    data = (Real*)(chunk->getRawData());
  else
    data = new Real[dataSize];

  // for reading from a character stream
  char *ch = (char*)data;

  cfdStream->read(ch, rank*chsize*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename DataGrid1d::value_type))
  {
    typename DataGrid1d::value_type *raw = chunk->getRawData();
    for (int64_t i=0; i<dataSize; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }
}
