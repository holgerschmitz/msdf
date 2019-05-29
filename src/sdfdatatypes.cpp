/*
 * sdfdatatypes.cpp
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */
#include "sdfdatatypes.hpp"

#include <schnek/typetools.hpp>
#include <schnek/vector.hpp>
#include <algorithm>

//==============================================================================
//==========================  SdfMeshVariable  =================================
//==============================================================================


SdfMeshVariable::SdfMeshVariable(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block_)
  : block(block_)
{
  this->readData(sdfStream, header, block);
}

pDataGrid1d SdfMeshVariable::get1dMesh(int i)
{
  if (rank!=1) throw GenericException("Wrong mesh dimension!");
  return mesh1d[i];
}

pDataGrid2d SdfMeshVariable::get2dMesh(int i)
{
  if (rank!=2) throw GenericException("Wrong mesh dimension!");
  return mesh2d[i];
}

pDataGrid3d SdfMeshVariable::get3dMesh(int i)
{
  if (rank!=3) throw GenericException("Wrong mesh dimension!");
  return mesh3d[i];
}

int SdfMeshVariable::getRank()
{
  return rank;
}

int SdfMeshVariable::getCount()
{
  return count;
}

double SdfMeshVariable::getMin(int i)
{
  switch (getRank())
  {
    case 1:
      return *std::min_element(this->mesh1d[i]->begin(), this->mesh1d[i]->end());
      break;
    case 2:
      return *std::min_element(this->mesh2d[i]->begin(), this->mesh2d[i]->end());
      break;
    case 3:
      return *std::min_element(this->mesh3d[i]->begin(), this->mesh3d[i]->end());
      break;
  }
}

double SdfMeshVariable::getMax(int i)
{
  switch (getRank())
  {
    case 1:
      return *std::max_element(this->mesh1d[i]->begin(), this->mesh1d[i]->end());
      break;
    case 2:
      return *std::max_element(this->mesh2d[i]->begin(), this->mesh2d[i]->end());
      break;
    case 3:
      return *std::max_element(this->mesh3d[i]->begin(), this->mesh3d[i]->end());
      break;
  }
}

void SdfMeshVariable::readData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block)
{
  sdfStream->seekg(block.getMetaDataOffset());
  rank = block.getNDims();

  this->readValue(*sdfStream, mult);
  this->readString(*sdfStream, units, 32);
  this->readString(*sdfStream, meshId, 32);

  switch (block.getDataType())
  {
    case sdf_real4:
      precision = 4;
      break;
    case sdf_real8:
      precision = 8;
      break;
    default:
      throw DataTypeUnsupportedException(block.getName(), block.getDataTypeStr());
  }

  if (precision==sizeof(float)) {
    std::cerr << "  reading single precision\n";
    switch (block.getBlockType())
    {
      case sdf_plain_variable:
        readDataByPrecision(sdfStream, header, block, schnek::Type2Type<float>());
        break;
      case sdf_point_variable:
        readParticleDataByPrecision(sdfStream, header, block, schnek::Type2Type<float>());
        break;
      case sdf_lagrangian_mesh:
        readLagrangianDataByPrecision(sdfStream, header, block, schnek::Type2Type<float>());
        break;
      default:
        break;
    }
  }
  else if (precision==sizeof(double)) {
    std::cerr << "  reading double precision\n";
    switch (block.getBlockType())
    {
      case sdf_plain_variable:
        readDataByPrecision(sdfStream, header, block, schnek::Type2Type<double>());
        break;
      case sdf_point_variable:
        readParticleDataByPrecision(sdfStream, header, block, schnek::Type2Type<float>());
        break;
      case sdf_lagrangian_mesh:
        readLagrangianDataByPrecision(sdfStream, header, block, schnek::Type2Type<double>());
        break;
      default:
        break;
    }
  }
  else throw GenericException("CFD file contains unsupported precision data!");
}


template<typename realtype>
void SdfMeshVariable::readDataByPrecision(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype rt)
{
  count = 1;
  std::cerr << "  rank is " << rank << "\n";
  switch (rank)
  {
    case 1:
      mesh1d = pDataGrid1d(new DataGrid1d());
      this->readMeshData(sdfStream, header, block, rt, *mesh1d);
      break;
    case 2:
      mesh2d = pDataGrid2d(new DataGrid2d());
      this->readMeshData(sdfStream, header, block, rt, *mesh2d);
      break;
    case 3:
      mesh3d = pDataGrid3d(new DataGrid3d());
      this->readMeshData(sdfStream, header, block, rt, *mesh3d);
      break;
    default:
      throw GenericException("CFD file contains mesh data with rank other than 1,2 or 3!");
      break;
  }
}


template<typename realtype, class GridType>
void SdfMeshVariable::readMeshData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype, GridType &grid)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<int32_t, GridType::Rank> Index;

  const int rank = GridType::Rank;
  Index arrsize;
  int32_t stagger;

  for (int i=0; i<rank; ++i) this->readValue(*sdfStream, arrsize[rank-1-i]);
  this->readValue(*sdfStream, stagger);

  sdfStream->seekg(block.getDataLocation());

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

  sdfStream->read(ch, length*sizeof(Real));

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
void SdfMeshVariable::readParticleDataByPrecision(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype rt)
{
  count = 1;
  typedef typename realtype::OriginalType Real;

  int64_t npart;
  this->readValue(*sdfStream, npart);

  sdfStream->seekg(block.getDataLocation());

  mesh1d->resize(GridIndex1d(npart));

  Real *data;
  if (sizeof(Real)==sizeof(typename DataGrid1d::value_type))
    data = (Real*)(mesh1d->getRawData());
  else
    data = new Real[npart];

  // for reading from a character stream
  char *ch = (char*)data;

  sdfStream->read(ch, npart*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename DataGrid1d::value_type))
  {
    typename DataGrid1d::value_type *raw = mesh1d->getRawData();
    for (int64_t i=0; i<npart; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }
}

template<typename realtype>
void SdfMeshVariable::readLagrangianDataByPrecision(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype rt)
{

  std::cerr << "  rank is " << rank << "\n";
  switch (rank)
  {
    case 1:
      mesh1d = pDataGrid1d(new DataGrid1d());
      this->readLagrangianMeshData(sdfStream, header, block, rt, *mesh1d);
      break;
    case 2:
      mesh2d = pDataGrid2d(new DataGrid2d());
      this->readLagrangianMeshData(sdfStream, header, block, rt, *mesh2d);
      break;
    case 3:
      mesh3d = pDataGrid3d(new DataGrid3d());
      this->readLagrangianMeshData(sdfStream, header, block, rt, *mesh3d);
      break;
    default:
      throw GenericException("CFD file contains mesh data with rank other than 1,2 or 3!");
      break;
  }
}

template<typename realtype, class GridType>
void SdfMeshVariable::readLagrangianMeshData(pIstream sdfStream, pSdfFileHeader header, SdfBlockHeader &block, realtype, GridType &grid)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<int32_t, GridType::Rank> Index;

  const int rank = GridType::Rank;
  Index arrsize;
  int32_t stagger;

  for (int i=0; i<rank; ++i) this->readValue(*sdfStream, arrsize[rank-1-i]);
  this->readValue(*sdfStream, stagger);

  sdfStream->seekg(block.getDataLocation());

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

  sdfStream->read(ch, length*sizeof(Real));

  // if GridType::value_type is the same as Real then we're done,
  // otherwise we now need to copy and delete the data pointer
  if (sizeof(Real)!=sizeof(typename GridType::value_type))
  {
    typename GridType::value_type *raw = grid.getRawData();
    for (int64_t i=0; i<length; ++i) raw[i] = data[i]; // type cast
    delete[] data;
  }

}

//==============================================================================
//==========================  SdfMeshVariableStream  ===========================
//==============================================================================

SdfMeshVariableStream::SdfMeshVariableStream(pIstream sdfStream_, pSdfFileHeader header_, const SdfBlockHeader &block_, int64_t chunkLength_)
    : sdfStream(sdfStream_),
      header(header_),
      block(block_),
      chunkLength(chunkLength_)
{
  this->initStream();
}

void SdfMeshVariableStream::initStream()
{
  sdfStream->seekg(block.getMetaDataOffset());
  rank = block.getNDims();

  this->readValue(*sdfStream, mult);
  this->readString(*sdfStream, units, 32);
  this->readString(*sdfStream, meshId, 32);

  switch (block.getDataType())
  {
    case sdf_real4:
      precision = 4;
      break;
    case sdf_real8:
      precision = 8;
      break;
    default:
      throw DataTypeUnsupportedException(block.getName(), block.getDataTypeStr());
  }

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
void SdfMeshVariableStream::initStreamByPrecision(realtype)
{
  int stringLength = header->getStringLength();

  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<Real, 2> Bounds;

  Bounds extents;

  this->readValue(*sdfStream, dataLength);
  dataLength *= rank;

  activeOffset = block.getDataLocation();
  activeCount = 0;

//  std::cerr << "activeOffset = " << activeOffset << "\n";
//  std::cerr << "activeCount = " << activeCount << "\n";
}

void SdfMeshVariableStream::getMeshChunk(pDataGrid1d chunk)
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
    sdfStream->seekg(activeOffset);
    if (precision==sizeof(float))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<float>());
    }
    else if (precision==sizeof(double))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<double>());
    }
    activeCount += chsize;
    activeOffset = sdfStream->tellg();
  }
  else
  {
    activeCount = dataLength+1;
  }
}


template<typename realtype>
void SdfMeshVariableStream::readChunkByPrecision(int64_t chsize, pDataGrid1d chunk, realtype)
{
  typedef typename realtype::OriginalType Real;
  Real *data;
  if (sizeof(Real)==sizeof(typename DataGrid1d::value_type))
    data = (Real*)(chunk->getRawData());
  else
    data = new Real[chsize];

  // for reading from a character stream
  char *ch = (char*)data;

  sdfStream->read(ch, chsize*sizeof(Real));

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
//==========================  SdfMeshStream  ===========================
//==============================================================================

SdfMeshStream::SdfMeshStream(pIstream sdfStream_, pSdfFileHeader header_, const SdfBlockHeader &block_, int64_t chunkLength_)
    : sdfStream(sdfStream_),
      header(header_),
      block(block_),
      chunkLength(chunkLength_)
{
  this->initStream();
}

void SdfMeshStream::initStream()
{
  sdfStream->seekg(block.getMetaDataOffset());
  rank = block.getNDims();

  mults.resize(rank);
  labels.resize(rank);
  units.resize(rank);
  minvals.resize(rank);
  maxvals.resize(rank);

  for (int i=0; i<rank; ++i) this->readValue(*sdfStream, mults[i]);
  for (int i=0; i<rank; ++i) this->readString(*sdfStream, labels[i], 32);
  for (int i=0; i<rank; ++i) this->readString(*sdfStream, units[i], 32);
  this->readValue(*sdfStream, geometry);
  for (int i=0; i<rank; ++i) this->readValue(*sdfStream, minvals[i]);
  for (int i=0; i<rank; ++i) this->readValue(*sdfStream, maxvals[i]);

  switch (block.getDataType())
  {
    case sdf_real4:
      precision = 4;
      break;
    case sdf_real8:
      precision = 8;
      break;
    default:
      throw DataTypeUnsupportedException(block.getName(), block.getDataTypeStr());
  }

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
void SdfMeshStream::initStreamByPrecision(realtype)
{
  typedef typename realtype::OriginalType Real;
  typedef schnek::Vector<Real, 2> Bounds;

  this->readValue(*sdfStream, dataLength);

  activeOffset = block.getDataLocation();
  activeCount = 0;
}

void SdfMeshStream::getMeshChunk(pDataGrid2d chunk)
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

    if (precision==sizeof(float))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<float>());
    }
    else if (precision==sizeof(double))
    {
      readChunkByPrecision(chsize, chunk, schnek::Type2Type<double>());
    }
    activeCount += chsize;
  }
  else
  {
    activeCount = dataLength+1;
  }
}


template<typename realtype>
void SdfMeshStream::readChunkByPrecision(int64_t chsize, pDataGrid2d chunk, realtype)
{
  typedef typename realtype::OriginalType Real;
  Real *data = new Real[chsize];
  DataGrid2d &chunkGrid = *chunk;

  int64_t blocksize = dataLength*sizeof(Real);
  // for reading from a character stream
  char *ch = (char*)data;

//  std::cerr << "Reading Chunk:\n" <<
//      "  chunk size:  " << chsize << "\n" <<
//      "  data length: " << dataLength << "\n" <<
//      "  block size:  " << blocksize << "\n" <<
//      "  current pos: " << sdfStream->tellg() << "\n";


  for (int r=0; r<rank; ++r)
  {
//    std::cerr << "    seeking to " << activeOffset + r*blocksize << "\n";
    sdfStream->seekg(activeOffset + r*blocksize);
    sdfStream->read(ch, chsize*sizeof(Real));

    for (int i=0; i<chsize; ++i) chunkGrid(r,i) = data[i];
//    std::cerr << "    new pos " << sdfStream->tellg() << "\n";
  }

  activeOffset = activeOffset + chsize*sizeof(Real);
//  std::cerr << "  active offset: " << activeOffset << "\n";

  delete[] data;
}
