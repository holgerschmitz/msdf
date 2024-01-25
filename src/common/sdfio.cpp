/*
 * sdfio.cpp
 *
 *  Created on: 25 Jan 2024
 *      Author: Holger Schmitz
 *       Email: holger.schmitz@stfc.ac.uk
 */

#include "sdfio.hpp"

SdfBlockType intToBlockType(int type)
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

SdfDataType intToDataType(int type)
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

std::string BlockTypeToString(SdfBlockType type)
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

std::string DataTypeToString(SdfDataType type)
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

