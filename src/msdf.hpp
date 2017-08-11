/*
 * msdf.hpp
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef MSDF_MSDF_HPP_
#define MSDF_MSDF_HPP_

#include <schnek/grid.hpp>
#include <schnek/grid/gridcheck.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#ifdef NDEBUG
#define MsdfGridChecker schnek::GridNoArgCheck
#else
#define MsdfGridChecker schnek::GridAssertCheck
#endif

/// One dimensional data grids
typedef schnek::Grid<double, 1, MsdfGridChecker> DataGrid1d;
/// Two dimensional data grids
typedef schnek::Grid<double, 2, MsdfGridChecker> DataGrid2d;
/// Three dimensional data grids
typedef schnek::Grid<double, 3, MsdfGridChecker> DataGrid3d;

/// A shared pointer for one dimensional data grids
typedef boost::shared_ptr<DataGrid1d> pDataGrid1d;
/// A shared pointer for two dimensional data grids
typedef boost::shared_ptr<DataGrid2d> pDataGrid2d;
/// A shared pointer for three dimensional data grids
typedef boost::shared_ptr<DataGrid3d> pDataGrid3d;

/// The index type for one dimensional data grids
typedef DataGrid1d::IndexType GridIndex1d;
/// The index type for two dimensional data grids
typedef DataGrid2d::IndexType GridIndex2d;
/// The index type for three dimensional data grids
typedef DataGrid3d::IndexType GridIndex3d;

/// A one dimensional array of integer values
typedef schnek::Grid<int, 1, MsdfGridChecker> IntArray;
/// A shared pointer to a one dimensional array of integer values
typedef boost::shared_ptr<IntArray> pIntArray;

/// A one dimensional STL vector of integer values
typedef std::vector<int> IntVector;
/// A shared pointer to a one dimensional STL vector of integer values
typedef boost::shared_ptr<IntVector> pIntVector;

/// A one dimensional STL vector of double values
typedef std::vector<double> DoubleVector;
/// A shared pointer to a one dimensional STL vector of double values
typedef boost::shared_ptr<DoubleVector> pDoubleVector;

/// The electron mass in SI units
static const double massEl = 9.10938188e-31;
/// The proton mass in SI units
static const double massProton = 1.67262158e-27;
/// The Boltzmann constant in SI units
static const double kB = 1.3806503e-23;



#endif /* MSDF_MSDF_HPP_ */
