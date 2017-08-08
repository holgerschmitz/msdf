/*
 * penergy.cpp
 *
 *  Created on: 08 Mar 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "phaseplot.hpp"
#include "particlestream.hpp"
#include "hdfstream.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


namespace po = boost::program_options;

inline double McfdCommand_phaseplot::getValue(int axis, double px, double py, double pz, double x, double y)
{
  int id;

  switch (axis)
  {
    case 0: id = xAxisId;
      break;
    case 1: id = yAxisId;
      break;
    case 2: id = momentId;
      break;
  }

  switch (id)
  {
    case  0: return x;
    case  1: return y;
    case  2: return px;
    case  3: return py;
    case  4: return pz;
    case  5:
    {
      double p2 = px*px + py*py + pz*pz;
      // Using the identity
      // (gamma-1) = (gamma^2-1) / (gamma+1) = p^2  / (gamma+1)
      // to avoid rounding errors for small velocities
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

void McfdCommand_phaseplot::parseNumberList(std::string s, std::vector<double> &v)
{
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(",");

  tokenizer tokens(s, sep);
  int spId=0;
  for (tokenizer::iterator tok_iter = tokens.begin();
      tok_iter != tokens.end(); ++tok_iter, ++spId)
  {
    if (v.size() <= spId) v.resize(spId+1, 1.0);
    try {
      v[spId] *= boost::lexical_cast<double>(*tok_iter);
    }
    catch (boost::bad_lexical_cast)
    {
      std::cerr << "ERROR: Could not convert argument " << spId+1 << " in list: '"<< *tok_iter << "'\n";
    }

    if (!batch) std::cout << "Got number " << v[spId] << "\n";
  }
}

McfdCommand_phaseplot::McfdCommand_phaseplot()
  : option_desc("Options for the 'phaseplot' command")
{
  streamFact.addMesh().addSpecies().addMomentum().addWeight().setProgramOptions(option_desc);
  option_desc.add_options()
    ("xaxis", po::value<std::string>(&xAxis),"specifies the variable to be plotted on the x-axis. Any of x,y,px,py,pz,E,Ex,Ey,Ez,v,vx,vy,vz,theta,theta2,pxt,pyt,pzt (default: 'x')")
    ("yaxis", po::value<std::string>(&yAxis),"specifies the variable to be plotted on the y-axis. Any of x,y,px,py,pz,E,Ex,Ey,Ez,v,vx,vy,vz,theta,theta2,pxt,pyt,pzt (default: 'y')")
    ("moment", po::value<std::string>(&moment),"specifies the moment to be integrated. Any of 1,x,y,px,py,pz,E,Ex,Ey,Ez,v,vx,vy,vz,theta,theta2,pxt,pyt,pzt (default: '1')")
    ("xdim", po::value<int>(&xdim),"dimensions of the output data grid in the x-direction (default: 1024)")
    ("ydim", po::value<int>(&ydim),"dimensions of the output data grid in the y-direction (default: 1024)")
    ("xrmin", po::value<std::string>(&xrminStr),"minimum of the plot's x-range (default: 0.0)")
    ("xrmax", po::value<std::string>(&xrmaxStr),"maximum of the plot's x-range (default: 0.0)")
    ("yrmin", po::value<std::string>(&yrminStr),"minimum of the plot's y-range (default: 0.0)")
    ("yrmax", po::value<std::string>(&yrmaxStr),"maximum of the plot's y-range (default: 0.0)")
    ("xsmin", po::value<std::string>(&xsminStr),"minimum of the physical x-range from which to consider particles (default: 0.0)")
    ("xsmax", po::value<std::string>(&xsmaxStr),"maximum of the physical x-range from which to consider particles (default: 0.0)")
    ("ysmin", po::value<std::string>(&ysminStr),"minimum of the physical y-range from which to consider particles (default: 0.0)")
    ("ysmax", po::value<std::string>(&ysmaxStr),"maximum of the physical y-range from which to consider particles (default: 0.0)")
    ("mingamma", po::value<double>(&minGamma),"minimum energy of the particles to plot (default: 0.0)")
    ("maxgamma", po::value<double>(&maxGamma),"maximum energy of the particles to plot (default: 0.0 no restriction)")
    ("posPx", "If specified, only consider particles with positive px")
    ("output,o", po::value<std::string>(&outputName),"base name of the output file. A # in the file name will be replaced with the species number. If no # is present, the species number will be appended to the file name.")
    ("batch,b", "create output for batch processing of data.");

  option_pos.add("input", 1);
}


void McfdCommand_phaseplot::execute(int argc, char **argv)
{
  double minGamma2, maxGamma2;
  typedef schnek::Array<double,2> Coord;
  std::vector<pDataGrid2d> plots;
  std::vector<Coord> mins;
  std::vector<Coord> maxs;
  std::vector<Coord> dx;
  std::vector<bool> minMaxSet;
  
  int maxId = 0;
  int smallId = 0;
  long maxPos = 0;
  long pos = 0;
  
//  typedef boost::shared_ptr<std::ofstream> pofstream;
//  std::vector<pofstream> output_files;
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  batch = (vm.count("batch")>0);
  bool allPx = vm.count("posPx")<1;

  limitX = false;
  limitY = false;


  if (vm.count("output")<1) outputName = "phaseplot#.dat";
  if (vm.count("xaxis")<1) xAxis = "x";
  if (vm.count("yaxis")<1) yAxis = "y";
  if (vm.count("moment")<1) moment = "1";
  if (vm.count("mingamma")<1) minGamma = 0.0;
  if (vm.count("maxgamma")<1) maxGamma = 0.0;
  if (vm.count("xdim")<1) xdim = 1024;
  if (vm.count("ydim")<1) ydim = 1024;


  std::vector<double> xrmin, xrmax;
  std::vector<double> yrmin, yrmax;

  std::vector<double> xsmin, xsmax;
  std::vector<double> ysmin, ysmax;

  if (vm.count("xrmin")>0)
  {
    if (!batch) std::cout << "Parsing x range minimum "<< xrminStr <<"\n";
    parseNumberList(xrminStr, xrmin);
  }

  if (vm.count("xrmax")>0)
  {
    if (!batch) std::cout << "Parsing x range maximum "<< xrmaxStr <<"\n";
    parseNumberList(xrmaxStr, xrmax);
  }
  if (vm.count("yrmin")>0)
  {
    if (!batch) std::cout << "Parsing y range minimum "<< yrminStr <<"\n";
    parseNumberList(yrminStr, yrmin);
  }

  if (vm.count("yrmax")>0)
  {
    if (!batch) std::cout << "Parsing y range maximum "<< yrmaxStr <<"\n";
    parseNumberList(yrmaxStr, yrmax);
  }


  if (vm.count("xsmin")>0)
  {
    if (!batch) std::cout << "Parsing x range minimum "<< xsminStr <<"\n";
    parseNumberList(xsminStr, xsmin);
  }

  if (vm.count("xsmax")>0)
  {
    if (!batch) std::cout << "Parsing x range maximum "<< xsmaxStr <<"\n";
    parseNumberList(xsmaxStr, xsmax);
  }
  if (vm.count("ysmin")>0)
  {
    if (!batch) std::cout << "Parsing y range minimum "<< ysminStr <<"\n";
    parseNumberList(ysminStr, ysmin);
  }

  if (vm.count("ysmax")>0)
  {
    if (!batch) std::cout << "Parsing y range maximum "<< ysmaxStr <<"\n";
    parseNumberList(ysmaxStr, ysmax);
  }

  std::cout <<"SIZE: "<< xsmin.size() << " " << xsmax.size() << " " << ysmin.size() << " " << ysmax.size() << " \n";

  limitX = std::max(xrmin.size(), xrmax.size());
  limitY = std::max(yrmin.size(), yrmax.size());

  minGamma2 = minGamma*minGamma;
  maxGamma2 = maxGamma*maxGamma;
  
  xAxisId = makeAxisId(xAxis);
  yAxisId = makeAxisId(yAxis);
  momentId = makeAxisId(moment, true);
  

  // set up species arrays and calculate min and max values
  std::cerr << "set up species arrays and calculate min and max values\n";
  pParticleStream pstream = streamFact.getParticleStream(vm);
  if (!pstream)
  {
    print_help();
    exit(-1);
  }
  pstream->getNextChunks();

  while (! pstream->eos() )
  {
    GridIt1 sp_it = pstream->species->cbegin();
    GridIt1 px_it = pstream->px->cbegin();
    GridIt1 py_it = pstream->py->cbegin();
    GridIt1 pz_it = pstream->pz->cbegin();
    GridIt2 mesh_it = pstream->mesh->cbegin();
    
    int rank = pstream->getRank();

    while (sp_it != pstream->species->cend())
    {

      double px = *px_it;
      double py = *py_it;
      double pz = *pz_it;
      double x = *mesh_it;
      double y;
      double z;
      if (rank>1)
      {
        ++mesh_it;
        y = *mesh_it;
      }
      if (rank>2)
      {
        ++mesh_it;
        z = *mesh_it;
      }

      int id = *sp_it;
      if (id > maxId)
      {
        for (int i=maxId; i<id; ++i)
        {
//          pofstream ofile = pofstream(new std::ofstream(this->createOutputFile(i).c_str()));
//          output_files.push_back(ofile);
          mins.push_back(Coord(0,0));
          maxs.push_back(Coord(0,0));
          minMaxSet.push_back(false);
          pDataGrid2d pGrid(new DataGrid2d(GridIndex2d(xdim,ydim)));
          *pGrid = 0;
          plots.push_back(pGrid);
        }
        maxId = id;
      }

      id = id-1; // get in line with C indexing

      if (( (id >= xsmin.size()) || (x>xsmin[id]) ) &&
          ( (id >= xsmax.size()) || (x<xsmax[id]) ) &&
          ( (rank<2) ||
            ( ((id >= ysmin.size()) || (y>ysmin[id]) ) &&
              ((id >= ysmax.size()) || (y<ysmax[id]) ))) &&
          ( allPx || (px>0)) )
      {
        if (id < 0) ++smallId;
        else {

          double X = getValue(0, px, py, pz, x, y);
          double Y = getValue(1, px, py, pz, x, y);

          Coord minC = mins[id];
          Coord maxC = maxs[id];
          if (minMaxSet[id])
          {
            minC[0] = std::min(minC[0],X);
            minC[1] = std::min(minC[1],Y);
            maxC[0] = std::max(maxC[0],X);
            maxC[1] = std::max(maxC[1],Y);
          }
          else
          {
            minC[0] = X;
            minC[1] = Y;
            maxC[0] = X;
            maxC[1] = Y;
            minMaxSet[id] = true;
          }
          mins[id] = minC;
          maxs[id] = maxC;

          maxPos = pos;
        }
      }
      ++sp_it;
      ++mesh_it;
      ++px_it;
      ++py_it;
      ++pz_it;
      ++pos;
    }
  std::cerr << "+";
    pstream->getNextChunks();
  std::cerr << "-";
  }
  

  
  if (smallId>0) {
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";
    smallId = 1; 
  }
  
  
  dx.resize(mins.size());
  for (int i=0; i<mins.size(); ++i)
  {
    if (xrmin.size()>i) mins[i][0] = xrmin[i];
    if (xrmax.size()>i) maxs[i][0] = xrmax[i];
    if (yrmin.size()>i) mins[i][1] = yrmin[i];
    if (yrmax.size()>i) maxs[i][1] = yrmax[i];

    dx[i] = Coord( (maxs[i][0]-mins[i][0])/xdim, (maxs[i][1]-mins[i][1])/ydim );

    std::cerr << "Species " << i << ": min=("<<mins[i][0]<<","<<mins[i][1]
      <<") max=("<<maxs[i][0]<<","<<maxs[i][1]
      <<") dx=("<<dx[i][0]<<","<<dx[i][1]<< ")\n";
  }
  

  pstream = streamFact.getParticleStream(vm);

  int rank = pstream->getRank();

  if (!pstream)
  {
    print_help();
    exit(-1);
  }
  pstream->getNextChunks();

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
      double y;
      double z;
      if (rank>1)
      {
        ++mesh_it;
        y = *mesh_it;
      }
      if (rank>2)
      {
        ++mesh_it;
        z = *mesh_it;
      }

      int id = *sp_it - 1;
      if (( (id >= xsmin.size()) || (x>xsmin[id]) ) &&
          ( (id >= xsmax.size()) || (x<xsmax[id]) ) &&
          ( (rank<2) ||
            ( ((id >= ysmin.size()) || (y>ysmin[id]) ) &&
              ((id >= ysmax.size()) || (y<ysmax[id]) ))) &&
          ( allPx || (px>0)) )
      {

        if (id < 0) ++smallId;
        else {
          double gamma2 = 1 + px*px + py*py + pz*pz;
          if ((gamma2 >= minGamma2) && ((maxGamma<1.0) || (gamma2<=maxGamma2)))
          {

            double X = getValue(0, px, py, pz, x, y);
            double Y = getValue(1, px, py, pz, x, y);
            double Mom = getValue(2, px, py, pz, x, y);

            double xpic = (X - mins[id][0])/dx[id][0];
            double ypic = (Y - mins[id][1])/dx[id][1];

            int xbin = floor(xpic);
            int ybin = floor(ypic);

            double x_frac = xpic - xbin;
            double y_frac = ypic - ybin;

            if (xbin<0) { xbin=0; x_frac=0;}
            if (ybin<0) { ybin=0; y_frac=0;}
            if (xbin>=xdim-1)  { xbin=xdim-2; x_frac=1;}
            if (ybin>=ydim-1)  { ybin=ydim-2; y_frac=1;}

            DataGrid2d &grid = *plots[id];
            grid(xbin  ,ybin)   += Mom * (*wg_it) * (1-x_frac) * (1-y_frac);
            grid(xbin+1,ybin)   += Mom * (*wg_it) * x_frac     * (1-y_frac);
            grid(xbin  ,ybin+1) += Mom * (*wg_it) * (1-x_frac) * y_frac;
            grid(xbin+1,ybin+1) += Mom * (*wg_it) * x_frac     * y_frac;

  /*
            int xbin = floor(xpic + 0.5);
            int ybin = floor(ypic + 0.5);

            double x_frac = xpic - xbin;
            double y_frac = ypic - ybin;

            double gx[5], gy[5];
            double cf2 = x_frac*x_frac;
            double t1 = (0.5 + x_frac);
            t1 = t1*t1;
            gx(-2) = t1*t1;
            gx(-1) = 4.75 + 11.0*x_frac + 4.0*cf2*(1.5 - x_frac - cf2);
            gx( 0) = 14.375 + 6.0*cf2*(cf2 - 2.5);
            gx( 1) = 4.75 - 11.0*x_frac + 4.0*cf2*(1.5 + x_frac - cf2);
            t1 = (0.5_num - x_frac);
            t1 = t1*t1;
            gx( 2) = t1*t1;

            cf2 = y_frac*y_frac;
            t1 = (0.5 + y_frac);
            t1 = t1*t1;
            gy(-2) = t1*t1;
            gy(-1) = 4.75 + 11.0*y_frac + 4.0*cf2*(1.5 - y_frac - cf2);
            gy( 0) = 14.375 + 6.0*cf2*(cf2 - 2.5);
            gy( 1) = 4.75 - 11.0*y_frac + 4.0*cf2*(1.5 + y_frac - cf2);
            t1 = (0.5_num - y_frac);
            t1 = t1*t1;
            gy( 2) = t1*t1;
   */
          }

          maxPos = pos;
        }
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

  for (int i=0; i<plots.size(); ++i) {
    std::string outputName = createOutputFile(i);
    HDFostream output(outputName.c_str());
    output << *plots[i];
    output.close();
  }

  std::cout << "Successfully written " << plots.size() << " phase space plots" << std::endl;

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

}


char McfdCommand_phaseplot::makeAxisId(std::string axisStr, bool allowUnity)
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

void McfdCommand_phaseplot::print_help()
{
  std::cout << "\n  Manipulate cfd files: creates a phase-plot for species and stores in hdf5 format\n\n  Usage:\n"
        << "    mcfd phaseplot [options] <input>\n\n"
        << "  where <input> is the name of the cfd/raw file.\n\n";

  std::cout << option_desc;
}

std::string McfdCommand_phaseplot::createOutputFile(int speciesId)
{
  std::string speciesIdStr = boost::lexical_cast<std::string>(speciesId);
  std::string result = boost::replace_first_copy(outputName,"#",speciesIdStr);
  if (result == outputName) result = outputName + speciesIdStr;
  return result;
}
