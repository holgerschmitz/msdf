/*
 * dataio.h
 *
 *  Created on: 11 Jan 2012
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef DATAIO_H_
#define DATAIO_H_

#include "cfdfile.hpp"
#include "cfddatatypes.hpp"

#include "sdffile.hpp"
#include "sdfdatatypes.hpp"

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

class MeshDataImpl
{
  public:
    virtual void readData() = 0;
    virtual int getRank() = 0;
    virtual pDataGrid1d get1dMesh() = 0;
    virtual pDataGrid2d get2dMesh() = 0;
    virtual pDataGrid3d get3dMesh() = 0;
    virtual double getMin() = 0;
    virtual double getMax() = 0;
};
typedef boost::shared_ptr<MeshDataImpl> pMeshDataImpl;

class CfdMeshDataImpl : public MeshDataImpl
{
  public:
    CfdMeshDataImpl(std::string inputName_, std::string blockName_);
    void readData();
    int getRank();
    pDataGrid1d get1dMesh();
    pDataGrid2d get2dMesh();
    pDataGrid3d get3dMesh();
    double getMin();
    double getMax();
  private:
    pCfdFile cfdFile;
    pCfdBlockData data;
    CfdMeshVariable *mData;

    std::string blockName;
    std::string inputName;
};

class SdfMeshDataImpl : public MeshDataImpl
{
  public:
    SdfMeshDataImpl(std::string inputName_, std::string blockName_);
    void readData();
    int getRank();
    pDataGrid1d get1dMesh();
    pDataGrid2d get2dMesh();
    pDataGrid3d get3dMesh();
    double getMin();
    double getMax();
  private:
    pSdfFile sdfFile;
    pSdfBlockData data;
    SdfMeshVariable *mData;

    std::string blockName;
    std::string inputName;
};

class MeshData
{
  private:
    pMeshDataImpl impl;

    std::string blockName;
    std::string inputName;

  public:
    MeshData() {};
    void setProgramOptions(boost::program_options::options_description &option_desc,
        boost::program_options::positional_options_description &option_pos);
    bool isValid(boost::program_options::variables_map &vm);
    void readData(boost::program_options::variables_map &vm);
    std::string getInputName() { return inputName; }
    std::string getBlockName() { return blockName; }

    int getRank() { return impl->getRank(); }
    pDataGrid1d get1dMesh() { return impl->get1dMesh(); }
    pDataGrid2d get2dMesh() { return impl->get2dMesh(); }
    pDataGrid3d get3dMesh() { return impl->get3dMesh(); }
    double getMin() { return impl->getMin(); }
    double getMax() { return impl->getMax(); }
};


#endif /* DATAIO_H_ */
