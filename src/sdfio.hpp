/*
 * sdfio.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFIO_H_
#define SDFIO_H_

#include "cfdio.hpp"
#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

typedef enum {sdf_scrubbed, sdf_null_block, sdf_plain_mesh, sdf_point_mesh, sdf_plain_variable,
    sdf_point_variable, sdf_constant, sdf_array, sdf_run_info, sdf_source,
    sdf_stitched_tensor, sdf_stitched_material, sdf_stitched_matvar, sdf_stitched_species,
    sdf_particle_species, sdf_other_block} SdfBlockType;

typedef enum {sdf_null_type, sdf_integer4, sdf_integer8, sdf_real4, sdf_real8, sdf_real16,
        sdf_character, sdf_logical, sdf_other_type} SdfDataType;

typedef enum {sdf_null_geom, sdf_cartesian, sdf_cylindrical, sdf_spherical} SDFGeometryType;

class SdfIo : public BinaryIo
{
  protected:

    SdfBlockType intToBlockType(int type) const
    {
      switch (type)
      {
        case -1: return sdf_scrubbed;
        case  0: return sdf_null_block;
        case  1: return sdf_plain_mesh;
        case  2: return sdf_point_mesh;
        case  3: return sdf_plain_variable;
        case  4: return sdf_point_variable;
        case  5: return sdf_constant;
        case  6: return sdf_array;
        case  7: return sdf_run_info;
        case  8: return sdf_source;
        case  9: return sdf_stitched_tensor;
        case 10: return sdf_stitched_material;
        case 11: return sdf_stitched_matvar;
        case 12: return sdf_stitched_species;
        case 13: return sdf_particle_species;
        default: return sdf_other_block;
      }
    }

    SdfDataType intToDataType(int type) const
    {
      switch (type)
      {
        case  0: return sdf_null_type;
        case  1: return sdf_integer4;
        case  2: return sdf_integer8;
        case  3: return sdf_real4;
        case  4: return sdf_real8;
        case  5: return sdf_real16;
        case  6: return sdf_character;
        case  7: return sdf_logical;
        case  8:
        default: return sdf_other_type;
      }
    }

    std::string BlockTypeToString(SdfBlockType type) const
    {
      switch (type)
      {
        case sdf_scrubbed:
          return "scrubbed";
        case sdf_null_block:
          return "null";
        case sdf_plain_mesh:
          return "plain_mesh";
        case sdf_point_mesh:
          return "point_mesh";
        case sdf_plain_variable:
          return "plain_variable";
        case sdf_point_variable:
          return "point_variable";
        case sdf_constant:
          return "constant";
        case sdf_array:
          return "array";
        case sdf_run_info:
          return "run_info";
        case sdf_source:
          return "source";
        case sdf_stitched_tensor:
          return "stitched_tensor";
        case sdf_stitched_material:
          return "stitched_material";
        case sdf_stitched_matvar:
          return "stitched_matvar";
        case sdf_stitched_species:
          return "stitched_species";
        case sdf_particle_species:
          return "particle_species";
        case sdf_other_block:
        default:
          return "other";
      }
    }


    std::string DataTypeToString(SdfDataType type) const
    {
      switch (type)
      {
        case sdf_null_type:
          return "null";
        case sdf_integer4:
          return "integer4";
        case sdf_integer8:
          return "integer8";
        case sdf_real4:
          return "real4";
        case sdf_real8:
          return "real8";
        case sdf_real16:
          return "real16";
        case sdf_character:
          return "character";
        case sdf_logical:
          return "logical";
        case sdf_other_type:
        default:
          return "other";
      }
    }
};

class SdfBlockData : public SdfIo
{
  public:
    virtual SdfBlockType getBlockType() = 0;
};
typedef boost::shared_ptr<SdfBlockData> pSdfBlockData;


class SdfBlockDataStream : public SdfIo
{
  public:
    virtual SdfBlockType getBlockType() = 0;
};
typedef boost::shared_ptr<SdfBlockDataStream> pSdfBlockDataStream;

#endif /* SDFIO_H_ */
