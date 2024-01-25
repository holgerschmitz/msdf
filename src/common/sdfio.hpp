/*
 * sdfio.h
 *
 *  Created on: 18 Oct 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef MSDF_SDFIO_H_
#define MSDF_SDFIO_H_

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
 * Convert an integer to an SDF block type
 *
 * @param type  the integer code from the SDF file
 * @returns     the block type
 */
SdfBlockType intToBlockType(int type);

/**
 * Convert an integer to an SDF data type
 *
 * @param type  the integer code from the SDF file
 * @returns     the block type
 */
SdfDataType intToDataType(int type);

/**
 * Convert an SDF block type to a string
 *
 * @param type  the block type
 * @returns     the human readable name of the block type
 */
std::string BlockTypeToString(SdfBlockType type);

/**
 * Convert an SDF data type to a string
 *
 * @param type  the data type
 * @returns     the human readable name of the data type
 */
std::string DataTypeToString(SdfDataType type);

/**
 * Interface for SDF block data
 */
class SdfBlockData
{
  public:
    virtual ~SdfBlockData() {}
    virtual SdfBlockType getBlockType() = 0;
};
typedef boost::shared_ptr<SdfBlockData> pSdfBlockData;


class SdfBlockDataStream
{
  public:
    virtual ~SdfBlockDataStream() {}
    virtual SdfBlockType getBlockType() = 0;
};

typedef boost::shared_ptr<SdfBlockDataStream> pSdfBlockDataStream;

#endif /* SDFIO_H_ */
