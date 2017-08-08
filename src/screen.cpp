/*
 * screen.cpp
 *
 *  Created on: 4 Jul 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */


#include "screen.hpp"
#include "particlestream.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


namespace po = boost::program_options;

inline double McfdCommand_screen::getValue(double px, double py, double pz, double x, double y)
{
  switch (momentId)
  {
    case  0: return x;
    case  1: return y;
    case  2: return px;
    case  3: return py;
    case  4: return pz;
    case  5:
    {
      double p2 = px*px + py*py + pz*pz;
      return p2/(sqrt(1+p2) + 1);
    }
    case  6: return px*px/(sqrt(1+px*px) + 1);
    case  7: return py*py/(sqrt(1+py*py) + 1);
    case  8: return pz*pz/(sqrt(1+pz*pz) + 1);
    case  100: return 1;
    default: return x;
  }

}

void McfdCommand_screen::parseNumberList(std::string s, std::vector<double> &v)
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

McfdCommand_screen::McfdCommand_screen()
  : option_desc("Options for the 'phaseplot' command")
{
  streamFact.addMesh().addSpecies().addMomentum().addWeight().setProgramOptions(option_desc);
  option_desc.add_options()
    ("moment", po::value<std::string>(&moment),"specifies the moment to be integrated. Any of 1,x,y,px,py,pz,E,Ex,Ey,Ez (default: '1')")
    ("dim", po::value<int>(&dim),"dimensions of the output data grid in the x-direction (default: 1024)")
    ("xscreen", po::value<double>(&xscreen),"x-position of the screen (default: 0.0)")
    ("yrmin", po::value<std::string>(&yrminStr),"minimum of the plot's y-range (default: 0.0)")
    ("yrmax", po::value<std::string>(&yrmaxStr),"maximum of the plot's y-range (default: 0.0)")
    ("xsmin", po::value<std::string>(&xsminStr),"minimum of the physical x-range from which to consider particles (default: 0.0)")
    ("xsmax", po::value<std::string>(&xsmaxStr),"maximum of the physical x-range from which to consider particles (default: 0.0)")
    ("ysmin", po::value<std::string>(&ysminStr),"minimum of the physical y-range from which to consider particles (default: 0.0)")
    ("ysmax", po::value<std::string>(&ysmaxStr),"maximum of the physical y-range from which to consider particles (default: 0.0)")
    ("mingamma", po::value<double>(&minGamma),"minimum energy of the particles to plot (default: 0.0)")
    ("maxgamma", po::value<double>(&maxGamma),"maximum energy of the particles to plot (default: no maximum energy)")
    ("all", "If specified, consider all particles, otherwise only consider those moving towards the screen")
    ("output,o", po::value<std::string>(&outputName),"base name of the output file. A # in the file name will be replaced with the species number. If no # is present, the species number will be appended to the file name.")
    ("batch,b", "create output for batch processing of data.");

  option_pos.add("input", 1);
}


void McfdCommand_screen::execute(int argc, char **argv)
{
  double minGamma2, maxGamma2;
  typedef schnek::Array<double,1> Coord;
  std::vector<pDataGrid1d> plots;
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
  bool allParticles = vm.count("all")>0;

  limitX = false;


  if (vm.count("output")<1) outputName = "phaseplot#.dat";
  if (vm.count("moment")<1) moment = "1";
  if (vm.count("mingamma")<1) minGamma = 0.0;
  if (vm.count("maxgamma")<1) maxGamma = 0.0;
  if (vm.count("dim")<1) dim = 1024;
  if (vm.count("xscreen")<1) xscreen = 0.0;


  std::vector<double> yrmin, yrmax;

  std::vector<double> xsmin, xsmax;
  std::vector<double> ysmin, ysmax;

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

  limitX = std::max(yrmin.size(), yrmax.size());

  minGamma2 = minGamma*minGamma;
  maxGamma2 = maxGamma*maxGamma;

  momentId = makeAxisId(moment);


  // set up species arrays and calculate min and max values

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
//          pofstream ofile = pofstream(new std::ofstream(this->createOutputFile(i).c_str()));
//          output_files.push_back(ofile);
          mins.push_back(Coord(0));
          maxs.push_back(Coord(0));
          minMaxSet.push_back(false);
          pDataGrid1d pGrid(new DataGrid1d(GridIndex1d(dim)));
          *pGrid = 0;
          plots.push_back(pGrid);
        }
        maxId = id;
      }

      id = id-1; // get in line with C indexing

      if (( (id >= xsmin.size()) || (x>xsmin[id]) ) &&
          ( (id >= xsmax.size()) || (x<xsmax[id]) ) &&
          ( (id >= ysmin.size()) || (y>ysmin[id]) ) &&
          ( (id >= ysmax.size()) || (y<ysmax[id]) ) &&
          ( allParticles || (px*(xscreen-x)>=0)) )
      {
        if (id < 0) ++smallId;
        else {

          Coord minC = mins[id];
          Coord maxC = maxs[id];
          if (minMaxSet[id])
          {
            minC[0] = std::min(minC[0],y);
            maxC[0] = std::max(maxC[0],y);
          }
          else
          {
            minC[0] = y;
            maxC[0] = y;
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
    pstream->getNextChunks();
  }


  if (smallId>0) {
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";
    smallId = 1;
  }


  dx.resize(mins.size());
  for (int i=0; i<mins.size(); ++i)
  {
    if (yrmin.size()>i) mins[i][0] = yrmin[i];
    if (yrmax.size()>i) maxs[i][0] = yrmax[i];

    dx[i] = Coord((maxs[i][0]-mins[i][0])/dim );

    if (!batch)
      std::cerr << "Species " << i << ": min=("<<mins[i][0]
        <<") max=("<<maxs[i][0]
        <<") dx=("<<dx[i][0]<< ")\n";
  }


  pstream = streamFact.getParticleStream(vm);
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
      ++mesh_it;
      double y = *mesh_it;

      int id = *sp_it - 1;
      if (( (id >= xsmin.size()) || (x>xsmin[id]) ) &&
          ( (id >= xsmax.size()) || (x<xsmax[id]) ) &&
          ( (id >= ysmin.size()) || (y>ysmin[id]) ) &&
          ( (id >= ysmax.size()) || (y<ysmax[id]) ) &&
          ( allParticles || (px*(xscreen-x)>=0)) )
      {

        if (id < 0) ++smallId;
        else {
          double gamma2 = 1 + px*px + py*py + pz*pz;
          double yproj = y + (xscreen - x)*py/px;

          if ((gamma2 >= minGamma2) &&
              ((maxGamma2<1) || (gamma2<=maxGamma2)) &&
              (yproj >= mins[id][0]) &&
              (yproj <= maxs[id][0]))
          {
            double Mom = getValue(px, py, pz, x, y);


            double xpic = (yproj - mins[id][0])/dx[id][0];

            int xbin = floor(xpic);

            double x_frac = xpic - xbin;

            if (xbin<0) { xbin=0; x_frac=0;}
            if (xbin>=dim-1)  { xbin=dim-2; x_frac=1;}

            DataGrid1d &grid = *plots[id];
            grid(xbin  ) += Mom * (*wg_it) * (1-x_frac);
            grid(xbin+1) += Mom * (*wg_it) * x_frac;
            grid(xbin  ) += Mom * (*wg_it) * (1-x_frac);
            grid(xbin+1) += Mom * (*wg_it) * x_frac;
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
    std::ofstream output(outputName.c_str());
    DataGrid1d &grid = *plots[i];
    double h = dx[i][0];
    double m = mins[i][0];
    for (int j=0; j<dim; ++j) output << j*h + m << " " << grid[j] << "\n";
    output.close();
  }

  if (!batch) std::cout << "Successfully written " << plots.size() << " phase space plots" << std::endl;

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

}


char McfdCommand_screen::makeAxisId(std::string axisStr)
{
  if (axisStr == "1") return 100;
  if (axisStr == "x") return 0;
  if (axisStr == "y") return 1;
  if (axisStr == "px") return 2;
  if (axisStr == "py") return 3;
  if (axisStr == "pz") return 4;
  if (axisStr == "E") return 5;
  if (axisStr == "Ex") return 6;
  if (axisStr == "Ey") return 7;
  if (axisStr == "Ez") return 8;

  return 100;
}

void McfdCommand_screen::print_help()
{
  std::cout << "\n  Manipulate cfd files: creates a distribution of particles projected onto the screen at x=xscreen and writes the result into a gnuplot-readable ascii file\n\n  Usage:\n"
        << "    mcfd screen [options] <input>\n\n"
        << "  where <input> is the name of the cfd/raw file.\n\n";

  std::cout << option_desc;
}

std::string McfdCommand_screen::createOutputFile(int speciesId)
{
  std::string speciesIdStr = boost::lexical_cast<std::string>(speciesId);
  std::string result = boost::replace_first_copy(outputName,"#",speciesIdStr);
  if (result == outputName) result = outputName + speciesIdStr;
  return result;
}
