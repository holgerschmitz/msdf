/*
 * penergy.cpp
 *
 *  Created on: 14 Feb 2011
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "penergy.hpp"
#include "particlestream.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>


namespace po = boost::program_options;


void McfdCommand_penergy::parseNumberList(std::string s, std::vector<double> &v)
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

    //if (!batch) std::cout << "Got number " << v[spId] << "\n";
  }
}

McfdCommand_penergy::McfdCommand_penergy()
  : option_desc("Options for the 'penergy' command")
{
  streamFact.addMesh().addSpecies().addMomentum().addWeight().setProgramOptions(option_desc);

  option_desc.add_options()
    ("xsmin", po::value<std::string>(&xsminStr),"minimum of the physical x-range from which to consider particles (default: 0.0)")
    ("xsmax", po::value<std::string>(&xsmaxStr),"maximum of the physical x-range from which to consider particles (default: 0.0)")
    ("ysmin", po::value<std::string>(&ysminStr),"minimum of the physical y-range from which to consider particles (default: 0.0)")
    ("ysmax", po::value<std::string>(&ysmaxStr),"maximum of the physical y-range from which to consider particles (default: 0.0)")
    ("mf", po::value<std::string>(&mfString),"list of mass factors S (SI), e (electron) or p (proton) separated by commas (default: S)")
    ("mass,m", po::value<std::string>(&massString),"list of masses in units of the mass factors separated by commas (default: 1.0)")
    ("mingamma", po::value<double>(&minGamma),"minimum energy of the particles to plot (default: 0.0)")
    ("batch,b", "create output for batch processing of data.");

  option_pos.add("input", 1);
}


void McfdCommand_penergy::execute(int argc, char **argv)
{
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(option_desc).positional(option_pos).run(), vm);
  po::notify(vm);

  if (vm.count("mingamma")<1) minGamma = 0.0;
  bool batch = (vm.count("batch")>0);

  double minGamma2 = minGamma*minGamma;

  pParticleStream pstream = streamFact.getParticleStream(vm);
  if (!pstream)
  {
    print_help();
    exit(-1);
  }

  int maxId = 0;
  int smallId = 0;
  long maxPos = 0;
  long pos = 0;


  std::vector<double> speciesPx;
  std::vector<double> speciesPy;
  std::vector<double> speciesPz;
  std::vector<double> speciesPx2;
  std::vector<double> speciesPy2;
  std::vector<double> speciesPz2;
  std::vector<double> speciesNumber;
  std::vector<double> speciesCount;

  pstream->getNextChunks();
  std::vector<double> masses;

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(",");

  if (vm.count("mf")>0)
  {
    if (!batch) std::cout << "Parsing mass factors "<< mfString <<"\n";

    tokenizer mfTokens(mfString, sep);

    for (tokenizer::iterator tok_iter = mfTokens.begin();
        tok_iter != mfTokens.end(); ++tok_iter)
    {
      if (*tok_iter == "e") masses.push_back(massEl);
      else
      if (*tok_iter == "p") masses.push_back(massProton);
      else masses.push_back(1.0);
      if (!batch) std::cout << "Got mass factor " << masses.back() << "\n";
    }
  }

  if (vm.count("mass")>0)
  {
    if (!batch) std::cout << "Parsing masses "<< massString <<"\n";
    parseNumberList(massString, masses);
  }

  if (!batch)
  {
    for (int i=0; i<masses.size(); ++i)
      std::cout << "Got mass " << masses[i] << "\n";
  }

  std::vector<double> xsmin, xsmax;
  std::vector<double> ysmin, ysmax;

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
      int id = *sp_it;
      if (id > maxId)
      {
        // I know the 0 is redundant, but I'm paranoid!
        speciesPx.resize(id,0);
        speciesPy.resize(id,0);
        speciesPz.resize(id,0);
        speciesPx2.resize(id,0);
        speciesPy2.resize(id,0);
        speciesPz2.resize(id,0);
        speciesNumber.resize(id,0);
        speciesCount.resize(id,0);
        maxId = id;
      }
      if (masses.size() < id) masses.resize(id, 1.0);

      id = id-1; // get in line with C indexing

      if (id < 0) ++smallId;
      else {
        double mass = masses[id];
    	double weight = *wg_it;
    	double px = *px_it;
    	double py = *py_it;
    	double pz = *pz_it;
        double x = *mesh_it;
        ++mesh_it;
        double y = *mesh_it;


        if (( (id >= xsmin.size()) || (x>xsmin[id]) ) &&
            ( (id >= xsmax.size()) || (x<xsmax[id]) ) &&
            ( (id >= ysmin.size()) || (y>ysmin[id]) ) &&
            ( (id >= ysmax.size()) || (y<ysmax[id]) ) )
        {

          double gamma2 = 1 + px*px + py*py + pz*pz;
          if (gamma2 >= minGamma2)
          {
            if (!pstream->isRaw())
            {
              px /= mass;
              py /= mass;
              pz /= mass;
            }
            else
            {
              px *= 2.99792458e8;
              py *= 2.99792458e8;
              pz *= 2.99792458e8;
            }

    //    	std::cout << px << " " << py << " " << pz << " " << weight << "\n";

            speciesNumber[id] = speciesNumber[id] + weight;
            speciesCount[id] = speciesCount[id] + 1;

            speciesPx[id] = speciesPx[id] + px*weight;
            speciesPy[id] = speciesPy[id] + py*weight;
            speciesPz[id] = speciesPz[id] + pz*weight;

            speciesPx2[id] = speciesPx2[id] + px*px*weight;
            speciesPy2[id] = speciesPy2[id] + py*py*weight;
            speciesPz2[id] = speciesPz2[id] + pz*pz*weight;

            maxPos = pos;
          }
        }
      }
      ++sp_it;
      ++wg_it;
      ++mesh_it;
      ++px_it;
      ++py_it;
      ++pz_it;
      ++pos;
    }
    pstream->getNextChunks();
  }

  if (smallId>0)
    std::cerr << "WARNING!\n    " << smallId << " species IDs found with values < 1\n";

  if (vm.count("batch")<1)
  {
    std::cout << "Maximum valid particle index " << maxPos << std::endl;

    std::cout  << std::setiosflags(std::ios::left)
        << "  Specied Id         T_x            T_y            T_z            T"
        << "         U_x            U_y            U_z\n";
  }
  for (unsigned int id = 0; id<maxId; ++id)
  {
    double Px = speciesPx[id] / speciesNumber[id];
    double Py = speciesPy[id] / speciesNumber[id];
    double Pz = speciesPz[id] / speciesNumber[id];

    double Px2= speciesPx2[id] / speciesNumber[id];
    double Py2= speciesPy2[id] / speciesNumber[id];
    double Pz2= speciesPz2[id] / speciesNumber[id];

    double Tx = masses[id]*(Px2 - Px*Px)/kB;
    double Ty = masses[id]*(Py2 - Py*Py)/kB;
    double Tz = masses[id]*(Pz2 - Pz*Pz)/kB;
    std::cout << std::setiosflags(std::ios::right) << "  "
        << std::setw(6) << id+1  << "     "
        << std::setw(15) << Tx  << std::setw(15) << Ty  << std::setw(15) << Tz
        << std::setw(15) << (Tx+Ty+Tz)/3.0
        << std::setw(15) << Px  << std::setw(15) << Py  << std::setw(15) << Pz
        << std::setw(15) << speciesNumber[id] << "\n";
  }
}


void McfdCommand_penergy::print_help()
{
  std::cout << "\n  Manipulate cfd files: calculates (nonrelativistic!) thermal energies of particles of each species\n\n  Usage:\n"
        << "    mcfd penergy [options] <input>\n\n"
        << "  where <input> is the name of the cfd file.\n\n";

  std::cout << option_desc;
}
