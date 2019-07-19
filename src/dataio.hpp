/*
 * dataio.h
 *
 *  Created on: 11 Jan 2012
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef DATAIO_H_
#define DATAIO_H_

#include "common/sdffile.hpp"
#include "sdfdatatypes.hpp"

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

class MeshDataImpl
{
  public:
    virtual ~MeshDataImpl() {}
    virtual void readData() = 0;
    virtual int getRank() = 0;
    virtual int getCount() = 0;
    virtual pDataGrid1d get1dMesh(int i) = 0;
    virtual pDataGrid2d get2dMesh(int i) = 0;
    virtual pDataGrid3d get3dMesh(int i) = 0;
    virtual double getMin(int i) = 0;
    virtual double getMax(int i) = 0;
};
typedef boost::shared_ptr<MeshDataImpl> pMeshDataImpl;


class SdfMeshDataImpl : public MeshDataImpl
{
  public:
    SdfMeshDataImpl(std::string inputName_, std::string blockName_);
    void readData();
    int getRank();
    int getCount();
    pDataGrid1d get1dMesh(int i);
    pDataGrid2d get2dMesh(int i);
    pDataGrid3d get3dMesh(int i);
    double getMin(int i);
    double getMax(int i);
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
    int getCount() { return impl->getCount(); }
    pDataGrid1d get1dMesh(int i) { return impl->get1dMesh(i); }
    pDataGrid2d get2dMesh(int i) { return impl->get2dMesh(i); }
    pDataGrid3d get3dMesh(int i) { return impl->get3dMesh(i); }
    double getMin(int i) { return impl->getMin(i); }
    double getMax(int i) { return impl->getMax(i); }
};


#endif /* DATAIO_H_ */
