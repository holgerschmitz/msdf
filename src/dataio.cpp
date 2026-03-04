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
  if ((data->getBlockType() != sdf_plain_variable)
      && (data->getBlockType() != sdf_point_variable)
      && (data->getBlockType() != sdf_point_mesh))
    throw msdf::GenericException("Block contains a data type that is not compatible with this action!");

  if (data->getBlockType() == sdf_point_mesh)
  {
    mData = nullptr;
    pmData = dynamic_cast<SdfPointMesh*>(&(*data));
  }
  else
  {
    pmData = nullptr;
    mData = &(dynamic_cast<SdfMeshVariable&>(*data));
  }
}

int SdfMeshDataImpl::getRank() {
  if (pmData) return 1;  // point_mesh data is always returned as 1D arrays
  return mData->getRank();
}

int SdfMeshDataImpl::getCount() {
  if (pmData) return pmData->getCount();
  return mData->getCount();
}

pDataGrid1d SdfMeshDataImpl::get1dMesh(int i) {
  if (pmData) return pmData->get1dMesh(i);
  return mData->get1dMesh(i);
}

pDataGrid2d SdfMeshDataImpl::get2dMesh(int i) {
  if (pmData) throw msdf::GenericException("point_mesh data is 1D only");
  return mData->get2dMesh(i);
}

pDataGrid3d SdfMeshDataImpl::get3dMesh(int i) {
  if (pmData) throw msdf::GenericException("point_mesh data is 1D only");
  return mData->get3dMesh(i);
}

double SdfMeshDataImpl::getMin(int i) {
  if (pmData) return pmData->getMin(i);
  return mData->getMin(i);
}

double SdfMeshDataImpl::getMax(int i) {
  if (pmData) return pmData->getMax(i);
  return mData->getMax(i);
}

bool SdfMeshDataImpl::isPointMesh() const {
  return pmData != nullptr;
}


//===========================================================
//======================    MeshData    =====================
//===========================================================

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

//===========================================================
//====================    MultiMeshData    ==================
//===========================================================

void MultiMeshData::setProgramOptions(boost::program_options::options_description &option_desc,
    boost::program_options::positional_options_description &option_pos)
{
  option_desc.add_options()
      ("block,b", po::value<std::string>(&blockNamePattern),
        "A comma separated list of data block names to read from the SDF file. "
        "Each block will be read into a separate dataset in the HDF5 file with "
        "the same name as the block."
      )
      ("input,i", po::value<std::string>(&inputNamePattern),
        "File name pattern of the SDF files. Supports Kleene star wildcards, e.g. 'data*.sdf')."
      )
      ("x,x", po::value<int>(&xSlice)->default_value(-1),
        "Slice the input data in the x-direction at the given index."
      )
      ("y,y", po::value<int>(&ySlice)->default_value(-1),
        "Slice the input data in the y-direction at the given index."
      )
      ("z,z", po::value<int>(&zSlice)->default_value(-1),
        "Slice the input data in the z-direction at the given index."
      )
      ;

  option_pos.add("block", 1);
  option_pos.add("input", 2);
}

bool MultiMeshData::isValid(po::variables_map &vm)
{
  return (vm.count("input")>0) && (vm.count("block")>0);
}

void MultiMeshData::init()
{
  std::string blockName;
  std::string inputName;
  std::string::size_type pos = 0;
  while (pos < blockNamePattern.size())
  {
    std::string::size_type nextPos = blockNamePattern.find(',', pos);
    if (nextPos == std::string::npos)
      nextPos = blockNamePattern.size();
    blockName = blockNamePattern.substr(pos, nextPos-pos);
    blockNames.push_back(blockName);
    pos = nextPos+1;
  }

}

void MultiMeshData::readData(
    boost::program_options::variables_map &vm, 
    std::string theInputName, 
    std::string theBlockName
) 
{
  impl = pMeshDataImpl(new SdfMeshDataImpl(theInputName, theBlockName));
  impl->readData();
}
