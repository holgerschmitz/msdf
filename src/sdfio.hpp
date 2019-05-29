/*
 * sdfio.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef SDFIO_H_
#define SDFIO_H_

#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include "binaryio.hpp"

/**
 * Enumeration for SDF data block types
 */
typedef enum {sdf_scrubbed, sdf_null_block, sdf_plain_mesh, sdf_point_mesh, sdf_plain_variable,
    sdf_point_variable, sdf_constant, sdf_array, sdf_run_info, sdf_source,
    sdf_stitched_tensor, sdf_stitched_material, sdf_stitched_matvar, sdf_stitched_species,
    sdf_particle_species, sdf_plain_derived, sdf_point_derived, sdf_contiguous_tensor,
    sdf_contiguous_material, sdf_contiguous_matvar, sdf_contiguous_species, sdf_cpu_split,
    sdf_stitched_obstacle_group, sdf_unstructured_mesh, sdf_stitched, sdf_contiguous,
    sdf_lagrangian_mesh, sdf_station, sdf_station_derived, sdf_datablock, sdf_namevalue,
    sdf_other_block} SdfBlockType;

/**
 * Enumeration for SDF data types
 */
typedef enum {sdf_null_type, sdf_integer4, sdf_integer8, sdf_real4, sdf_real8, sdf_real16,
        sdf_character, sdf_logical, sdf_other_type} SdfDataType;

/**
 * Enumeration for SDF geometry types
 */
typedef enum {sdf_null_geom, sdf_cartesian, sdf_cylindrical, sdf_spherical} SDFGeometryType;

/**
 *
 */
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
        case 14: return sdf_plain_derived;
        case 15: return sdf_point_derived;
        case 16: return sdf_contiguous_tensor;
        case 17: return sdf_contiguous_material;
        case 18: return sdf_contiguous_matvar;
        case 19: return sdf_contiguous_species;
        case 20: return sdf_cpu_split;
        case 21: return sdf_stitched_obstacle_group;
        case 22: return sdf_unstructured_mesh;
        case 23: return sdf_stitched;
        case 24: return sdf_contiguous;
        case 25: return sdf_lagrangian_mesh;
        case 26: return sdf_station;
        case 27: return sdf_station_derived;
        case 28: return sdf_datablock;
        case 29: return sdf_namevalue;
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
        case sdf_plain_derived:
          return "plain_derived";
        case sdf_point_derived:
          return "point_derived";
        case sdf_contiguous_tensor:
          return "contiguous_tensor";
        case sdf_contiguous_material:
          return "contiguous_material";
        case sdf_contiguous_matvar:
          return "contiguous_matvar";
        case sdf_contiguous_species:
          return "contiguous_species";
        case sdf_cpu_split:
          return "cpu_split";
        case sdf_stitched_obstacle_group:
          return "stitched_obstacle_group";
        case sdf_unstructured_mesh:
          return "unstructured_mesh";
        case sdf_stitched:
          return "stitched";
        case sdf_contiguous:
          return "contiguous";
        case sdf_lagrangian_mesh:
          return "lagrangian_mesh";
        case sdf_station:
          return "station";
        case sdf_station_derived:
          return "station_derived";
        case sdf_datablock:
          return "datablock";
        case sdf_namevalue:
          return "namevalue";

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
    virtual ~SdfBlockData() {}
    virtual SdfBlockType getBlockType() = 0;
};
typedef boost::shared_ptr<SdfBlockData> pSdfBlockData;


class SdfBlockDataStream : public SdfIo
{
  public:
    virtual ~SdfBlockDataStream() {}
    virtual SdfBlockType getBlockType() = 0;
};

typedef boost::shared_ptr<SdfBlockDataStream> pSdfBlockDataStream;

#endif /* SDFIO_H_ */
