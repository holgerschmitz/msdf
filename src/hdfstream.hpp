/*
 * hdfstream.h
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef HDFSTREAM_H
#define HDFSTREAM_H
//-----------------------------------------------------------------------------

#define HSCHMITZ_OVERRIDE_H5_HAVE_PARALLEL

#include "mcfd.hpp"

#include <hdf5.h>

#include <iostream>


  /** @file hdfstream.h
    * @brief IO class for HDF
    *
    * IO classes for handling HDF files and streams
    */


//-----------------------------------------------------------------------------
//HDFstream


/** @brief IO class for handling HDF files
  *  
  * This is the abstract base class for HDF-IO- classes.
  * Implements the basic operations on HDFstreams as virtual methods.
  */
class HDFstream {
  protected:
    /// HDF5 File id
    hid_t       file_id; 
    
    /// HDF5 Error status
    herr_t      status;
    
    /// name of the datablock to be read or written
    std::string blockname;
    /// counter for the sets with a given blockname read from or written to the file
    int sets_count;  
    
    /// Specifies if the stream is active in this process (in case of parallel execution)
    bool active;
    bool activeModified;
    
  public:
    /// constructor 
    HDFstream();
    ///copy constructur 
    HDFstream(const HDFstream&);
    /// destructor
    virtual ~HDFstream();

    /// open  file 
    virtual int open(const char*)=0;
    
    /// close file 
    virtual void close();
    
    /// return true=1 if data are still available 
    virtual bool good() const;
    
    void setBlockName(std::string blockname_);
    /// assign 
    HDFstream& operator = (const HDFstream&);
    
    void setActive(bool active_) { active = active_; activeModified = true; }
    
  protected:
    std::string getNextBlockName();
};

//HDFstream
//---------------------------------------------------------------

/** @brief Input stream for HDF files */
class HDFistream : public HDFstream {
  public:
    /// constructor 
    HDFistream();
    
    /// copy constructor */
    HDFistream(const HDFistream&);
    
    /// constructor, opens HDF file "fname", selects first dataset 
    HDFistream(const char* fname);

    /// opens HDF file "fname", selects first dataset 
    virtual int open(const char*);
       
    /// stream input operator for a schnek::Matrix 
    template<typename TYPE, int RANK, template<int> class Checking>
    HDFistream& operator>>(schnek::Grid<TYPE, RANK, Checking>& grid);
};

//HDFistream
//-----------------------------------------------------------------------------

/** @brief output stream for HDF files */
class HDFostream : public HDFstream {
  public:
    /// constructor 
    HDFostream();
    
    /// copy constructor 
    HDFostream(const HDFostream&);
    
    /// constructor, opens HDF file "fname" 
    HDFostream(const char* fname);

    /// open file 
    virtual int open(const char*);
    
    /// stream output operator for a matrix
    template<typename TYPE, int RANK, template<int> class Checking>
    HDFostream& operator<< (const schnek::Grid<TYPE, RANK, Checking>& grid);
};

template<typename TYPE>
struct H5DataType{
  static const hid_t type;
};



//HDFostream
//-----------------------------------------------------------------------------

#include "hdfstream.t"

//-----------------------------------------------------------------------------
#endif // HDFSTREAM_H
