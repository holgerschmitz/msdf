/*
 * cfdio.h
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef CFDIO_H_
#define CFDIO_H_

#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>


typedef boost::shared_ptr<std::istream> pIstream;

typedef enum {scribble, additional, mesh, mesh_variable, snapshot,
    stitched_vector, stitched_magnitude, constant, arb_db, other_block} BlockType;

typedef enum { cartesian_mesh, particle_mesh, other_mesh } MeshType;

class BinaryIo
{
  protected:
    template<typename T>
    void readValue(std::istream &in, T &data)
    {
      void *ptr = &data;
      char *ch = (char*)ptr;

      in.read(ch,sizeof(T));
    }

    void readString(std::istream &in, std::string &str, int length)
    {
      char buffer[1024];
      in.read(buffer,length);
      str = std::string(buffer);
      boost::trim(str);
    }
};

class CfdIo : public BinaryIo
{
  protected:

    BlockType intToBlockType(int type) const
    {
      switch (type)
      {
        case -1: return scribble;
        case 0: return additional;
        case 1: return mesh;
        case 2: return mesh_variable;
        case 3: return snapshot;
        case 4: return stitched_vector;
        case 5: return stitched_magnitude;
        case 6: return constant;
        case 7: return arb_db;
        default: return other_block;
      }
    }

    MeshType intToMeshType(int type) const
    {
      switch (type)
      {
        case 0: return cartesian_mesh;
        case 1: return particle_mesh;
        default: return other_mesh;
      }
    }

     std::string BlockTypeToString(BlockType type) const
     {
       switch (type)
       {
         case scribble:
           return "scribble";
         case additional:
           return "additional";
         case mesh:
           return "mesh";
         case mesh_variable:
           return "mesh_variable";
         case snapshot:
           return "snapshot";
         case stitched_vector:
           return "stitched_vector";
         case stitched_magnitude:
           return "stitched_magnitude";
         case constant:
           return "constant";
         case arb_db:
           return "arb_db";
         case other_block:
         default:
           return "other";
       }
     }

     std::string MeshTypeToString(MeshType type) const
      {
        switch (type)
        {
          case cartesian_mesh:
            return "cartesian_mesh";
          case particle_mesh:
            return "particle_mesh";
          case other_mesh:
          default:
            return "other";
        }
      }
};

class CfdBlockData : public CfdIo
{
  public:
    virtual BlockType getBlockType() = 0;
};
typedef boost::shared_ptr<CfdBlockData> pCfdBlockData;


class CfdBlockDataStream : public CfdIo
{
  public:
    virtual BlockType getBlockType() = 0;
};
typedef boost::shared_ptr<CfdBlockDataStream> pCfdBlockDataStream;

class BlockNotFoundException
{
  private:
    std::string name;
  public:
    BlockNotFoundException(std::string name_) throw()
        : name(name_) {}

    BlockNotFoundException(const BlockNotFoundException &ex) throw()
        : name(ex.name) {}

    BlockNotFoundException &operator=(const BlockNotFoundException &ex) throw()
    { name = ex.name; return *this; }

    std::string getName() const throw()
    { return name;  }
};

class BlockTypeUnsupportedException
{
  private:
    std::string blockname;
    std::string blocktype;
  public:
    BlockTypeUnsupportedException(std::string blockname_, std::string blocktype_) throw()
        : blockname(blockname_), blocktype(blocktype_) {}

    BlockTypeUnsupportedException(const BlockTypeUnsupportedException &ex) throw()
        : blockname(ex.blockname), blocktype(ex.blocktype) {}

    BlockTypeUnsupportedException &operator=(const BlockTypeUnsupportedException &ex) throw()
    { blockname=ex.blockname; blocktype=ex.blocktype;  return *this; }

    std::string getBlockName() const throw()
    { return blockname;  }

    std::string getBlockType() const throw()
    { return blocktype;  }
};


class DataTypeUnsupportedException
{
  private:
    std::string blockname;
    std::string datatype;
  public:
    DataTypeUnsupportedException(std::string blockname_, std::string datatype_) throw()
        : blockname(blockname_), datatype(datatype_) {}

    DataTypeUnsupportedException(const DataTypeUnsupportedException &ex) throw()
        : blockname(ex.blockname), datatype(ex.datatype) {}

    DataTypeUnsupportedException &operator=(const DataTypeUnsupportedException &ex) throw()
    { blockname=ex.blockname; datatype=ex.datatype;  return *this; }

    std::string getBlockName() const throw()
    { return blockname;  }

    std::string getDataType() const throw()
    { return datatype;  }
};

class GenericException
{
  private:
    std::string message;
  public:
    GenericException(std::string message_) throw()
        : message(message_) {}

    GenericException(const GenericException &ex) throw()
        : message(ex.message) {}

    GenericException &operator=(const GenericException &ex) throw()
    { message = ex.message; return *this; }

    std::string getMessage() const throw()
    { return message;  }
};

#endif /* CFDIO_H_ */
