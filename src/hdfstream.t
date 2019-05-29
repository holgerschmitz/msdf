#include <sstream>

#include "binaryio.hpp"

template<typename TYPE, int RANK, template<int> class Checking>
HDFistream& HDFistream::operator>>(schnek::Grid<TYPE, RANK, Checking>& grid)
{
  if (!active) return *this;

  std::string dset_name = getNextBlockName();

  typedef typename schnek::Grid<TYPE, RANK, Checking>::IndexType IndexType;
  
  IndexType mdims = grid.getDims();
  IndexType mlow = grid.getLow();
  IndexType mhigh = grid.getHigh();
  
  hsize_t dims[RANK];
  hsize_t locdims[RANK];
  hsize_t start[RANK];
  
  TYPE *data = grid.getRawData();

  /* open the dataset collectively */
  hid_t dataset = H5Dopen(file_id, dset_name.c_str(), H5P_DEFAULT);
  if (dataset == -1) throw GenericException("Problems opening HDF file!");
  
  /* read the data on single processor */
  hid_t ret = H5Dread(dataset,
                      H5DataType<TYPE>::type, 
                      H5S_ALL, 
                      H5S_ALL,
                      H5P_DEFAULT,
                      data);
  if (ret == -1) throw GenericException("Problems reading data from HDF file!");

  /* close dataset collectively */
  ret=H5Dclose(dataset);
  if (ret == -1) throw GenericException("Problems closing HDF dataset!");

  return *this;
}

template<typename TYPE, int RANK, template<int> class Checking>
HDFostream& HDFostream::operator<< (const schnek::Grid<TYPE, RANK, Checking>& grid)
{
  if (!active) return *this;

  std::string dset_name = getNextBlockName();
  
  typedef typename schnek::Grid<TYPE, RANK, Checking>::IndexType IndexType;
  
  IndexType mdims = grid.getDims();
  IndexType mlow = grid.getLo();
  IndexType mhigh = grid.getHi();
  
  hsize_t dims[RANK];
  hsize_t locdims[RANK];
  hsize_t start[RANK];
  
  bool empty = false;
  
  for (int i=0; i<RANK; ++i) locdims[i] = mdims[i];
  
  const TYPE *data = grid.getRawData();
  hid_t ret;
  
  /* setup dimensionality object */
  hid_t sid = H5Screate_simple (RANK, locdims, NULL);

  /* create a dataset collectively */
  hid_t dataset = H5Dcreate(
      file_id,
      dset_name.c_str(),
      H5DataType<TYPE>::type,
      sid,
      H5P_DEFAULT, 
      H5P_DEFAULT, 
      H5P_DEFAULT);

  /* write data on single processor */
  ret = H5Dwrite(
      dataset,
      H5DataType<TYPE>::type,
      H5S_ALL,
      H5S_ALL,
      H5P_DEFAULT,
      data);
  if (ret == -1) throw GenericException("Problems writing data to HDF file!");

  /* close dataset collectively */					    
  ret=H5Dclose(dataset);
  if (ret == -1) throw GenericException("Problems closing HDF dataset!");
    
  /* release all IDs created */
  H5Sclose(sid);
    
  return *this;
}

