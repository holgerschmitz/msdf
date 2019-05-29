/*
 * penergy.cpp
 *
 *  Created on: 08 Mar 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "angular.hpp"
#include "particlestream.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


namespace po = boost::program_options;

inline double MsdfCommand_angular::getValue(int axis, double px, double py, double pz, double x, double y)
{
  int id;
  if (axis==0) id = xAxisId;
  else id = yAxisId;

  switch (id)
  {
    case 0: return x;
    case 1: return y;
    case 2: return px;
    case 3: return py;
    case 4: return pz;
  }
  return x;
}

MsdfCommand_angular::MsdfCommand_angular()
  : option_desc("Options for the 'phaseplot' command")
{
  streamFact.addMesh().addSpecies().addMomentum().addWeight().setProgramOptions(option_desc);
  option_desc.add_options()
    ("xaxis", po::value<std::string>(&xAxis),"specifies the variable to be plotted on the x-axis. Any of x,y,px,py,pz (default: 'px')")
    ("yaxis", po::value<std::string>(&yAxis),"specifies the variable to be plotted on the y-axis. Any of x,y,px,py,pz (default: 'py')")
    ("xrmin", po::value<double>(&xrmin),"minimum of the x-range from which to consider particles (default: 0.0)")
    ("xrmax", po::value<double>(&xrmax),"maximum of the x-range from which to consider particles (default: 0.0)")
    ("yrmin", po::value<double>(&yrmin),"minimum of the y-range from which to consider particles (default: 0.0)")
    ("yrmax", po::value<double>(&yrmax),"maximum of the y-range from which to consider particles (default: 0.0)")
    ("dim", po::value<int>(&dim),"dimensions of the output data grid or number of bins (default: 1000)")
    ("mingamma", po::value<double>(&minGamma),"minimum energy of the particles to plot. A value less than 1.0 means no limiting (default: 0.0)")
    ("maxgamma", po::value<double>(&maxGamma),"maximum energy of the particles to plot. A value less than 1.0 means no limiting (default: 0.0)")
    ("output,o", po::value<std::string>(&outputName),"base name of the output file. A # in the file name will be replaced with the species number. If no # is present, the species number will be appended to the file name.")
    ("stretch", "stretch the data by the magnitude, i.e. calculate the weighted average or current density");

  option_pos.add("input", 1);
}


void MsdfCommand_angular::execute(int argc, char **argv)
{
  double minGamma2;
  double maxGamma2;
  std::vector<pDataGrid1d> plots;
  
  int maxId = 0;
  int smallId = 0;
  long maxPos = 0;
  long pos = 0;
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  limitX = false;
  limitY = false;

  if (vm.count("output")<1) outputName = "phaseplot#.dat";
  if (vm.count("xaxis")<1) xAxis = "px";
  if (vm.count("yaxis")<1) yAxis = "py";
  if (vm.count("chunk")<1) chunkLength = 1024*1024;
  if (vm.count("mingamma")<1) minGamma = 0.0;
  if (vm.count("maxgamma")<1) maxGamma = 0.0;
  if (vm.count("dim")<1) dim = 1000;
  
  if (vm.count("xrmin")<1) xrmin = 0.0;
  if (vm.count("xrmax")<1) xrmax = 0.0;
  if (vm.count("yrmin")<1) yrmin = 0.0;
  if (vm.count("yrmax")<1) yrmax = 0.0;
  
  limitX = !( (vm.count("xrmin")<1) && (vm.count("xrmax")<1) );
  limitY = !( (vm.count("yrmin")<1) && (vm.count("yrmax")<1) );

  minGamma2 = minGamma*minGamma;
  maxGamma2 = maxGamma*maxGamma;

  std::cerr << "minGamma - 1 " << minGamma-1 << "\n";
  std::cerr << "maxGamma - 1 " << maxGamma-1 << "\n";
  
  xAxisId = makeAxisId(xAxis);
  yAxisId = makeAxisId(yAxis);
  
  // set up species arrays and calculate min and max values
  
  pParticleStream pstream = streamFact.getParticleStream(vm);
  if (!pstream)
  {
    print_help();
    exit(-1);
  }
  pstream->getNextChunks();

  weightIt = vm.count("stretch")>0;

  while (! pstream->eos() )
  {
    GridIt1 sp_it = pstream->species->cbegin();
    GridIt1 wg_it = pstream->weight->cbegin();
    GridIt1 px_it = pstream->px->cbegin();
    GridIt1 py_it = pstream->py->cbegin();
    GridIt1 pz_it = pstream->pz->cbegin();
    GridIt2 mesh_it = pstream->mesh->cbegin();
    
    while (sp_it != pstream->species->cend())
    {

      double px = *px_it;
      double py = *py_it;
      double pz = *pz_it;
      double x = *mesh_it;
      ++mesh_it;
      double y = *mesh_it;

      int id = *sp_it;
      if (id > maxId)
      {
        for (int i=maxId; i<id; ++i)
        {
          pDataGrid1d pGrid(new DataGrid1d(GridIndex1d(dim+1)));
          *pGrid = 0.0;
          plots.push_back(pGrid);
        }
        maxId = id;
      }

      id = id-1; // get in line with C indexing

      if (id < 0) ++smallId;
      else {
        double mc = 9.10938188e-31*2.99792458e8;
        double gamma2 = 1 + (px*px + py*py + pz*pz)/(mc*mc);
        if (gamma2 >= minGamma2 && ((maxGamma<1.0) || (gamma2 <= maxGamma2)))
	{
	  
	  double X = getValue(0, px, py, pz, x, y);
	  double Y = getValue(1, px, py, pz, x, y);

          if (( !limitX || ((x > xrmin) && (x < xrmax)) ) &&
              ( !limitY || ((y > yrmin) && (y < yrmax)) ) )
          {
            double angle = dim*(atan2(Y,X)/(2.0*M_PI) + 0.5);
            double weight = (*wg_it);

            if (weightIt)
            {
              weight *= sqrt(X*X + Y*Y);
            }

            int bin = int(angle);
            double frac = angle - bin;

            if (bin<0.0) { bin = 0.0; frac = 0.0; }
            if (bin>=dim) { bin = dim; frac = 1.0; }

            DataGrid1d &grid = *plots[id];
            grid(bin)    += weight * (1-frac);
            grid(bin+1)  += weight * frac;
          }
	}
	
        maxPos = pos;
      }
      ++sp_it;
      ++mesh_it;
      ++px_it;
      ++py_it;
      ++pz_it;
      ++wg_it;
      ++pos;
    }
    pstream->getNextChunks();
  }

  for (int id=0; id<plots.size(); ++id) {
    std::string outputName = createOutputFile(id);
    std::ofstream output(outputName.c_str());
    DataGrid1d &grid = *plots[id];
    
    grid(0) += grid(dim);
    grid(dim) = grid(0);
    
    for (int i=0; i<=dim; ++i)
    {
      double angle = M_PI*(2.0*double(i)/double(dim) - 1.0);
      double r = grid(i);
      double x = r*cos(angle);
      double y = r*sin(angle);
      output << x << " " << y << " " << angle << " " << r << std::endl;
    }

    output.close();
  }

  std::cout << "Successfully written " << plots.size() << " phase space plots" << std::endl;

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

}




char MsdfCommand_angular::makeAxisId(std::string axisStr)
{
  if (axisStr == "x") return 0;
  if (axisStr == "y") return 1;
  if (axisStr == "px") return 2;
  if (axisStr == "py") return 3;
  if (axisStr == "pz") return 4;
  return 0;
}

void MsdfCommand_angular::print_help()
{
  std::cout << "\n  Manipulate cfd files: creates an angular distribution plot for each species and stores in plain ascii format\n\n  Usage:\n"
        << "    mcfd angular [options] <input>\n\n"
        << "  where <input> is the name of the cfd/raw file.\n\n";

  std::cout << option_desc;
  
  std::cout << "\nxrmin, xrmax, yrmin, yrmax define a region in real space to which the particles are limited.\n"
        << "  If neither xrmin nor xrmax are set, no limitations in the x-direction is made\n"
        << "  and equivalently for yrmin and yrmax\n";
}

std::string MsdfCommand_angular::createOutputFile(int speciesId)
{
  std::string speciesIdStr = boost::lexical_cast<std::string>(speciesId);
  std::string result = boost::replace_first_copy(outputName,"#",speciesIdStr);
  if (result == outputName) result = outputName + speciesIdStr;
  return result;
}
