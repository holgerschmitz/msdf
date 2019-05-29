#include "hdfstream.hpp"

#include "binaryio.hpp"

HDFstream::HDFstream()
  : file_id(-1),
    status(0),
    blockname("data"),
    sets_count(0),
    active(true),
    activeModified(false)
{}

HDFstream::HDFstream(const HDFstream& hdf)
  : file_id(hdf.file_id),
    status(hdf.status),
    blockname(hdf.blockname),
    sets_count(hdf.sets_count),
    active(true),
    activeModified(false)
{}

HDFstream &HDFstream::operator=(const HDFstream& hdf)
{
  file_id = hdf.file_id;
  status = hdf.status;
  sets_count = hdf.sets_count;
  blockname = hdf.blockname;
  active = hdf.active;
  activeModified = hdf.activeModified;
  return *this;
}

HDFstream::~HDFstream()
{
  close();
}

void HDFstream::close()
{
  if (file_id >= 0) {
    H5Fclose (file_id);
  }
  file_id = -1;
}

bool HDFstream::good() const
{
  return (file_id>=0);
}

void HDFstream::setBlockName(std::string blockname_)
{
  blockname = blockname_;
  std::replace(blockname.begin(), blockname.end(),'/',':');
  sets_count = -1;
}

std::string HDFstream::getNextBlockName()
{
  std::ostringstream bname;
  bname << blockname;

  if (sets_count<0) sets_count = 1;
  else
  {
    bname << sets_count++;
  }
  return bname.str();
}

// ----------------------------------------------------------------------

HDFistream::HDFistream() 
  : HDFstream() {}

HDFistream::HDFistream(const char* fname)
  : HDFstream()
{
  open(fname);
}

HDFistream::HDFistream(const HDFistream& hdf)
  : HDFstream(hdf)
{}

int HDFistream::open(const char* fname)
{
  close();

  if (active)
    file_id = H5Fopen (fname, H5F_ACC_RDONLY, H5P_DEFAULT);

  sets_count = 0;
  return 1;
}


// ----------------------------------------------------------------------

HDFostream::HDFostream()
   : HDFstream()
{}

HDFostream::HDFostream(const HDFostream& hdf)
  : HDFstream(hdf)
{}

HDFostream::HDFostream(const char* fname)
   : HDFstream()
{
  open(fname);
}

int HDFostream::open(const char* fname)
{
  sets_count = 0;
  
  if (active)
    file_id = H5Fcreate (fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  return file_id;
}

// ----------------------------------------------------------------------

template<>
const hid_t H5DataType<int>::type = H5T_NATIVE_INT;

template<>
const hid_t H5DataType<float>::type = H5T_NATIVE_FLOAT;

template<>
const hid_t H5DataType<double>::type = H5T_NATIVE_DOUBLE;


