/*
 * distfunc.cpp
 *
 *  Created on: 27 Sep 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "distfunc.hpp"
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

inline double McfdCommand_distfunc::getValue(int axis, double px, double py, double pz, double x, double y)
{
  int id;

  switch (axis)
  {
    case 0: id = axisId;
      break;
    case 1: id = momentId;
      break;
  }

  switch (id)
  {
    case 0: return x;
    case 1: return y;
    case 2: return px;
    case 3: return py;
    case 4: return pz;
    case  5:
    {
      double p2 = px*px + py*py + pz*pz;
      return p2/(sqrt(1+p2) + 1);
    }
    case  6: return px*px/(sqrt(1+px*px) + 1);
    case  7: return py*py/(sqrt(1+py*py) + 1);
    case  8: return pz*pz/(sqrt(1+pz*pz) + 1);
    case  9:
    {
      double p2 = px*px + py*py + pz*pz;
      return sqrt(p2)/sqrt(1+p2);
    }
    case 10: return px/sqrt(1+px*px + py*py + pz*pz);
    case 11: return py/sqrt(1+py*py + py*py + pz*pz);
    case 12: return pz/sqrt(1+pz*pz + py*py + pz*pz);
    case 13: return atan2(py,px);
    case 14:
    {
      double theta = atan2(py,px);
      return theta*theta;
    }
    case 15: return sqrt(py*py+pz*pz);
    case 16: return sqrt(px*px+pz*pz);
    case 17: return sqrt(px*px+py*py);
    case  100: return 1;
    default: return x;
  }
  
}

McfdCommand_distfunc::McfdCommand_distfunc()
  : option_desc("Options for the 'distfunc' command")
{
  streamFact.addMesh().addSpecies().addMomentum().addWeight().setProgramOptions(option_desc);

  option_desc.add_options()
    ("axis",   po::value<std::string>(&axis),"specifies the variable to be plotted on the x-axis. Any of x,y,px,py,pz,E,Ex,Ey,Ez,v,vx,vy,vz,theta,theta2,pxt,pyt,pzt (default: 'px')")
    ("moment", po::value<std::string>(&moment),"specifies the moment to be integrated. Any of 1,x,y,px,py,pz,E,Ex,Ey,Ez,v,vx,vy,vz,theta,theta2,pxt,pyt,pzt (default: '1')")
    ("xrmin", po::value<double>(&xrmin),"minimum of the x-range from which to consider particles (default: 0.0)")
    ("xrmax", po::value<double>(&xrmax),"maximum of the x-range from which to consider particles (default: 0.0)")
    ("yrmin", po::value<double>(&yrmin),"minimum of the y-range from which to consider particles (default: 0.0)")
    ("yrmax", po::value<double>(&yrmax),"maximum of the y-range from which to consider particles (default: 0.0)")
    ("dmin", po::value<double>(&dmin),"minimum of the data range (default: 0.0)")
    ("dmax", po::value<double>(&dmax),"maximum of the data range (default: 1.0)")
    ("dim", po::value<int>(&dim),"dimensions of the output data grid or number of bins (default: 1000)")
    ("east","if specified, only consider particles moving 'east', i.e. in the positive x direction")
    ("mingamma", po::value<double>(&minGamma),"minimum energy of the particles to plot (default: 0.0)")
    ("maxgamma", po::value<double>(&maxGamma),"maximum energy of the particles to plot (default: 0.0)")
    ("lfactor", po::value<double>(&lfactor),"factor to multiply the lower energy particle weights by (default: 0.0). If this value is non-zero, mingamma does not act as a cut-off but as the boundary between a low-gamma and a high-gamma region of phase space.")
    ("output,o", po::value<std::string>(&outputName),"base name of the output file. A # in the file name will be replaced with the species number. If no # is present, the species number will be appended to the file name.");

  option_pos.add("input", 1);
}


void McfdCommand_distfunc::execute(int argc, char **argv)
{
  double minGamma2;
  double maxGamma2;
  std::vector<pDataGrid1d> plots;
  
  int maxId = 0;
  int smallId = 0;
  long maxPos = 0;
  long pos = 0;
  double xmin = 1e10;
  double xmax = -1e10;


  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  limitX = false;
  limitY = false;

  if (vm.count("output")<1) outputName = "distfunc#.dat";
  if (vm.count("axis")<1) axis = "px";
  if (vm.count("moment")<1) moment = "1";
  if (vm.count("mingamma")<1) minGamma = 0.0;
  if (vm.count("maxgamma")<1) maxGamma = 0.0;
  if (vm.count("dim")<1) dim = 1000;
  
  if (vm.count("xrmin")<1) xrmin = 0.0;
  if (vm.count("xrmax")<1) xrmax = 0.0;
  if (vm.count("yrmin")<1) yrmin = 0.0;
  if (vm.count("yrmax")<1) yrmax = 0.0;
  if (vm.count("dmin")<1) dmin = 0.0;
  if (vm.count("dmax")<1) dmax = 1.0;
  
  bool includeLowGamma = (vm.count("lfactor")>0);
  bool east = (vm.count("east")>0);

  limitX = !( (vm.count("xrmin")<1) && (vm.count("xrmax")<1) );
  limitY = !( (vm.count("yrmin")<1) && (vm.count("yrmax")<1) );

  minGamma2 = minGamma*minGamma;
  maxGamma2 = maxGamma*maxGamma;
  
  axisId = makeAxisId(axis);
  momentId = makeAxisId(moment, true);
  
  // set up species arrays and calculate min and max values

  pParticleStream pstream = streamFact.getParticleStream(vm);
  if (!pstream)
  {
    print_help();
    exit(-1);
  }
  pstream->getNextChunks();

  int i=0;
  while (! pstream->eos() )
  {
    GridIt1 sp_it = pstream->species->cbegin();
    GridIt1 wg_it = pstream->weight->cbegin();
    GridIt1 px_it = pstream->px->cbegin();
    GridIt1 py_it = pstream->py->cbegin();
    GridIt1 pz_it = pstream->pz->cbegin();
    GridIt2 mesh_it = pstream->mesh->cbegin();
    std::cout << "New Block!\n";

    while (sp_it != pstream->species->cend())
    {

      double px = *px_it;
      double py = *py_it;
      double pz = *pz_it;
      double x = *mesh_it;
      ++mesh_it;
      double y = *mesh_it;
    //  std::cout << i++ << ":  x=" << x << std::endl;
      if (px<xmin) xmin=px;
      if (px>xmax) xmax=px;

      int id = *sp_it;
      if (id > maxId)
      {
        for (int i=maxId; i<id; ++i)
        {
          pDataGrid1d pGrid(new DataGrid1d(GridIndex1d(dim)));
          *pGrid = 0;
          plots.push_back(pGrid);
        }
        maxId = id;
      }

      id = id-1; // get in line with C indexing

      if (id < 0) ++smallId;
      else {
        double gamma2 = 1 + px*px + py*py + pz*pz;
        bool plotValue = includeLowGamma;
        double weightFactor = lfactor;

        if ((gamma2 >= minGamma2) && ((maxGamma2<=1) || (gamma2 <= maxGamma2)))
        {
          plotValue = true;
          weightFactor = 1.0;
        }

        if ( plotValue &&
            ( !east || (px>0)) &&
            ( !limitX || ((x > xrmin) && (x < xrmax)) ) &&
            ( !limitY || ((y > yrmin) && (y < yrmax)) ) )
        {
          double X = getValue(0,px, py, pz, x, y);
          double Mom = getValue(1, px, py, pz, x, y);

          double data = dim * (X - dmin)/(dmax-dmin);
          double weight = weightFactor * (*wg_it);

          int bin = int(data);
          double frac = data - bin;
          if ((bin>=0.0) && (bin<dim))
          {
            DataGrid1d &grid = *plots[id];
            grid(bin) += Mom*weight;
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
    
    for (int i=0; i<dim; ++i)
    {
      double val = dmin + i*(dmax-dmin)/double(dim);
      output << val << " " << grid(i) << std::endl;
    }

    output.close();
  }

  std::cout << "Successfully written " << plots.size() << " phase space plots" << std::endl;
  std::cout << "xmin " << xmin << " xmax " << xmax << std::endl;

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

}

char McfdCommand_distfunc::makeAxisId(std::string axisStr, bool allowUnity)
{
  if ((axisStr == "1") && allowUnity) return 100;
  if (axisStr == "x") return 0;
  if (axisStr == "y") return 1;
  if (axisStr == "px") return 2;
  if (axisStr == "py") return 3;
  if (axisStr == "pz") return 4;
  if (axisStr == "E") return 5;
  if (axisStr == "Ex") return 6;
  if (axisStr == "Ey") return 7;
  if (axisStr == "Ez") return 8;
  if (axisStr == "v") return 9;
  if (axisStr == "vx") return 10;
  if (axisStr == "vy") return 11;
  if (axisStr == "vz") return 12;
  if (axisStr == "theta") return 13;
  if (axisStr == "theta2") return 14;
  if (axisStr == "pxt") return 15;
  if (axisStr == "pyt") return 16;
  if (axisStr == "pzt") return 17;

  if (allowUnity) return 100;
  else return 0;
}

void McfdCommand_distfunc::print_help()
{
  std::cout << "\n  Manipulate cfd files: creates an angular distribution plot for each species and stores in plain ascii format\n\n  Usage:\n"
        << "    mcfd angular [options] <input>\n\n"
        << "  where <input> is the name of the cfd/raw file.\n\n";

  std::cout << option_desc;
  
  std::cout << "\nxrmin, xrmax, yrmin, yrmax define a region in real space to which the particles are limited.\n"
        << "  If neither xrmin nor xrmax are set, no limitations in the x-direction is made\n"
        << "  and equivalently for yrmin and yrmax\n";
}

std::string McfdCommand_distfunc::createOutputFile(int speciesId)
{
  std::string speciesIdStr = boost::lexical_cast<std::string>(speciesId);
  std::string result = boost::replace_first_copy(outputName,"#",speciesIdStr);
  if (result == outputName) result = outputName + speciesIdStr;
  return result;
}
