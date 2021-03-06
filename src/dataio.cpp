/*
 * dataio.cpp
 *
 *  Created on: 11 Jan 2012
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "dataio.hpp"

namespace po = boost::program_options;


SdfMeshDataImpl::SdfMeshDataImpl(std::string inputName_, std::string blockName_)
  : inputName(inputName_), blockName(blockName_)
{}

void SdfMeshDataImpl::readData()
{
  sdfFile = pSdfFile(new SdfFile(inputName));

  data = sdfFile->getBlockHeader(blockName)->getData(*sdfFile);
  if ((data->getBlockType() != sdf_plain_variable) && (data->getBlockType() != sdf_point_variable))
    throw GenericException("Block contains a data type that in not compatible with this action!");

  mData = &(dynamic_cast<SdfMeshVariable&>(*data));
}

int SdfMeshDataImpl::getRank() { return mData->getRank(); }
int SdfMeshDataImpl::getCount() { return mData->getCount(); }
pDataGrid1d SdfMeshDataImpl::get1dMesh(int i) { return mData->get1dMesh(i); }
pDataGrid2d SdfMeshDataImpl::get2dMesh(int i) { return mData->get2dMesh(i); }
pDataGrid3d SdfMeshDataImpl::get3dMesh(int i) { return mData->get3dMesh(i); }
double SdfMeshDataImpl::getMin(int i) { return mData->getMin(i); }
double SdfMeshDataImpl::getMax(int i) { return mData->getMax(i); }


void MeshData::setProgramOptions(boost::program_options::options_description &option_desc,
    boost::program_options::positional_options_description &option_pos)
{
  option_desc.add_options()
      ("block,b", po::value<std::string>(&blockName),"name of the data block to read from the SDF file")
      ("input,i", po::value<std::string>(&inputName),"name of the SDF file");

  option_pos.add("block", 1);
  option_pos.add("input", 2);
}

bool MeshData::isValid(po::variables_map &vm)
{
  return (vm.count("input")>0) && (vm.count("block")>0);
}

void MeshData::readData(po::variables_map &vm)
{
  impl = pMeshDataImpl(new SdfMeshDataImpl(inputName, blockName));
  impl->readData();
}
