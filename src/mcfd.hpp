/*
 * mcfd.h
 *
 *  Created on: 5 Oct 2010
 *      Author: Holger Schmitz
 */

#ifndef MCFD_H_
#define MCFD_H_

#include <schnek/grid.hpp>
#include <schnek/grid/gridcheck.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#ifdef NDEBUG
#define McfdGridChecker schnek::GridNoArgCheck
#else
#define McfdGridChecker schnek::GridAssertCheck
#endif

typedef schnek::Grid<double, 1, McfdGridChecker> DataGrid1d;
typedef schnek::Grid<double, 2, McfdGridChecker> DataGrid2d;
typedef schnek::Grid<double, 3, McfdGridChecker> DataGrid3d;

typedef boost::shared_ptr<DataGrid1d> pDataGrid1d;
typedef boost::shared_ptr<DataGrid2d> pDataGrid2d;
typedef boost::shared_ptr<DataGrid3d> pDataGrid3d;

typedef DataGrid1d::IndexType GridIndex1d;
typedef DataGrid2d::IndexType GridIndex2d;
typedef DataGrid3d::IndexType GridIndex3d;

typedef schnek::Grid<int, 1, McfdGridChecker> IntArray;
typedef boost::shared_ptr<IntArray> pIntArray;

typedef std::vector<int> IntVector;
typedef boost::shared_ptr<IntVector> pIntVector;

typedef std::vector<double> DoubleVector;
typedef boost::shared_ptr<DoubleVector> pDoubleVector;

static const double massEl = 9.10938188e-31;
static const double massProton = 1.67262158e-27;
static const double kB = 1.3806503e-23;



#endif /* MCFD_H_ */
