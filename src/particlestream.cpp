/*
 * particlestream.cpp
 *
 *  Created on: 19 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#include "particlestream.hpp"
#include "common/binaryio.hpp"
#include <ios>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

//===========================================================
//=================    SdfParticleStream    =================
//===========================================================

void SdfParticleStream::addMesh(std::string blockname)
{
  if (meshStream.get() == 0)
  {
    pSdfBlockHeader block = file->getBlockHeader(blockname);
    meshStream = pSdfMeshStream(
        new SdfMeshStream(file->getStream(), file->getHeader(), *block, chunkLength)
    );
    mesh = pDataGrid2d(new DataGrid2d());
  }
}

void SdfParticleStream::addSpecies(std::string blockname)
{
  this->addMesh(blockname);
  species = pDataGrid1d(new DataGrid1d());
}

void SdfParticleStream::addWeight(std::string blockname)
{
  pSdfBlockHeader block = file->getBlockHeader(blockname);
  weightStream = pSdfMeshVariableStream(
      new SdfMeshVariableStream(file->getStream(), file->getHeader(), *block, chunkLength)
  );
  weight = pDataGrid1d(new DataGrid1d());
}

void SdfParticleStream::addPx(std::string blockname)
{
  pSdfBlockHeader block = file->getBlockHeader(blockname);
  pxStream = pSdfMeshVariableStream(
      new SdfMeshVariableStream(file->getStream(), file->getHeader(), *block, chunkLength)
  );
  px = pDataGrid1d(new DataGrid1d());
}

void SdfParticleStream::addPy(std::string blockname)
{
  pSdfBlockHeader block = file->getBlockHeader(blockname);
  pyStream = pSdfMeshVariableStream(
      new SdfMeshVariableStream(file->getStream(), file->getHeader(), *block, chunkLength)
  );
  py = pDataGrid1d(new DataGrid1d());
}

void SdfParticleStream::addPz(std::string blockname)
{
  pSdfBlockHeader block = file->getBlockHeader(blockname);
  pzStream = pSdfMeshVariableStream(
      new SdfMeshVariableStream(file->getStream(), file->getHeader(), *block, chunkLength)
  );
  pz = pDataGrid1d(new DataGrid1d());
}

bool SdfParticleStream::eos()
{
  if (meshStream) return meshStream->eos();
  else return getValidVarStream()->eos();
}

pSdfMeshVariableStream SdfParticleStream::getValidVarStream()
{
  if (weightStream) return weightStream;
  if (pxStream) return pxStream;
  if (pyStream) return pyStream;
  if (pzStream) return pzStream;

  throw GenericException("No particle block specified in ParticleStream");
}

void SdfParticleStream::getNextChunks()
{
  if (eos()) return;
  if (meshStream) meshStream->getMeshChunk(mesh);
  if (weightStream) weightStream->getMeshChunk(weight);
  if (pxStream) pxStream->getMeshChunk(px);
  if (pyStream) pyStream->getMeshChunk(py);
  if (pzStream) pzStream->getMeshChunk(pz);
  if (species) {
    species->resize(GridIndex1d(mesh->getDims()[1]));
    (*species) = 1.0;
  }
}

//===========================================================
//=================    RawParticleStream    =================
//===========================================================


RawParticleStream::RawParticleStream(std::string file_, int64_t dataLength_)
    : file(file_),
      dataLength(dataLength_),
      end_reached(false)
{
//  stream.open((file + ".000").c_str());

//  readHeader();
  initStream();

  mesh = pDataGrid2d(new DataGrid2d());
  species = pDataGrid1d(new DataGrid1d());
  px = pDataGrid1d(new DataGrid1d());
  py = pDataGrid1d(new DataGrid1d());
  pz = pDataGrid1d(new DataGrid1d());
  weight = pDataGrid1d(new DataGrid1d());
  buffer.resize(GridIndex1d(6*dataLength));
  speciesBuffer.resize(GridIndex1d(dataLength));
}

bool RawParticleStream::eos()
{
  return end_reached;
}

void RawParticleStream::getNextChunks()
{
  if (chunk_size == 0) {
    end_reached = true;
    return;
  }
  float *data = (float*)buffer.getRawData();
  char *ch = (char*)data;
  int pos = 0;

  int64_t data_remainder = dataLength;

  while (chunk_remainder <= data_remainder)
  {
    stream.read(ch, 6*chunk_remainder*sizeof(float));
    for (int i=0; i<chunk_remainder; ++i) speciesBuffer(pos+i) = species_count;
    data_remainder -= chunk_remainder;
    ch += 6*chunk_remainder*sizeof(float);
    pos += chunk_remainder;

    advanceChunk();

    if (chunk_size==0) break;
  }

  if ((data_remainder != 0) && (chunk_size != 0))
  {
    stream.read(ch, 6*data_remainder*sizeof(float));
    for (int i=0; i<data_remainder; ++i) speciesBuffer(pos+i) = species_count;

    chunk_remainder -= data_remainder;
    data_remainder = 0;
  }

  int dataRead =  dataLength - data_remainder;
  if (species->getDims()[0] != dataRead)
  {
    mesh->resize(GridIndex2d(2,dataRead));
    weight->resize(GridIndex1d(dataRead));
    px->resize(GridIndex1d(dataRead));
    py->resize(GridIndex1d(dataRead));
    pz->resize(GridIndex1d(dataRead));
    species->resize(GridIndex1d(dataRead));
  }

  int bufPos = 0;
  char *ch_it = ch;
  for (int i=0; i<dataRead; ++i)
  {
    (*mesh)(0,i) = buffer(bufPos);
    (*mesh)(1,i) = buffer(bufPos+1);

    (*px)(i) = buffer(bufPos+2);
    (*py)(i) = buffer(bufPos+3);
    (*pz)(i) = buffer(bufPos+4);

    (*weight)(i) = buffer(bufPos+5);

    (*species)(i) = speciesBuffer(i);

    bufPos = bufPos + 6;
  }
}

void RawParticleStream::readHeader()
{
  msdf::detail::readValue(stream,count_i);
  msdf::detail::readValue(stream,rank);
  msdf::detail::readValue(stream,n_species);
  msdf::detail::readValue(stream,chunk);

  msdf::detail::readValue(stream,x_min);
  msdf::detail::readValue(stream,x_max);
  msdf::detail::readValue(stream,y_min);
  msdf::detail::readValue(stream,y_max);

  species_count = 0;
  chunk_size = 0;

  while ((chunk_size == 0) && (species_count < n_species)) readSpeciesHeader();
}

void RawParticleStream::readSpeciesHeader()
{
//  std::cout << "readSpeciesHeader = " << species_count << "\n";
  ++species_count;
  msdf::detail::readValue(stream,npleft);
  msdf::detail::readValue(stream,spcharge);
  msdf::detail::readValue(stream,spmass);

//  std::cout << "readSpeciesHeader = " << npleft<< " : " << spcharge << ":" << spmass << "\n";

  readChunkHeader();
}

void RawParticleStream::readChunkHeader()
{
  msdf::detail::readValue(stream,chunk_size);
  chunk_remainder = chunk_size;
//  std::cout << "readChunkHeader = " << chunk_size << "\n";
}

void RawParticleStream::advanceChunk()
{
//  std::cout << "advanceChunk = " << chunk << " : " << chunk_size << ":" << species_count <<"\n";
  if (chunk_size == chunk)
  {
    readChunkHeader();
  }
  else
  {
    chunk_size=0;
    chunk_remainder=0;
  }

  while ((chunk_size == 0) && ((fname_iter != filenames.end()) || (species_count < n_species)) )
  {
//    std::cout << "advanceChunk A\n";
    while ((chunk_size == 0) && (species_count < n_species)) readSpeciesHeader();
    if ((chunk_size == 0) && (fname_iter != filenames.end())) advanceStream();
  }

  if (chunk_size <0) exit(-1);
}

void RawParticleStream::initStream()
{
  int maxFNode = 0;
  int maxNode = 0;

  std::string filename = fs::path(file).filename().string();
  fs::path dir(file);
  dir.remove_filename();
  if (dir.empty()) dir /= ".";

//  std::cerr << "filename = " << filename << "\n";
//
//  std::cerr << "dir = " << dir.string() << "\n";
  if ( !fs::exists( dir ) ) {
    end_reached = true;
    return;
  }

  boost::regex fileRegex(filename + "\\.(\\d*)");
  boost::smatch regexMatch;


  fs::directory_iterator end_itr; // default construction yields past-the-end
  for ( fs::directory_iterator itr( dir );
        itr != end_itr;
        ++itr )
  {
//    std::cerr << itr->path() << "\n";
    if ( is_directory(itr->status()) ) continue;
    std::string entry = itr->path().filename().string();
    if(boost::regex_match(entry, regexMatch, fileRegex, boost::match_extra))
    {
      std::istringstream sstr(regexMatch[1]);
      int nnum;
      sstr >> nnum;
      if (maxFNode<nnum) maxFNode = nnum;
      filenames[nnum] = itr->path().string();
    }

  }
  fname_iter = filenames.begin();
  advanceStream();
}


void RawParticleStream::openStream()
{
  std::string fname = fname_iter->second;

  if (stream.is_open()) stream.close();
  std::cerr << "Reading file " << fname << "\n";
  stream.open(fname.c_str());
}

void RawParticleStream::advanceStream()
{
  if (fname_iter != filenames.end())
  {
    openStream();
    readHeader();
  }
  else
  {
    chunk_size=0;
    chunk_remainder=0;
  }
  ++fname_iter;
}


//===========================================================
//=================    ParticleStreamFactory    =================
//===========================================================

void ParticleStreamFactory::setProgramOptions(po::options_description &option_desc)
{
  option_desc.add_options()
      ("input,i", po::value<std::string>(&inputName),"name of the cfd file")
      ("chunk,c", po::value<int64_t>(&chunkLength),"chunk size used in buffered reading. Set this for optimising speed and memory usage.")
      ("raw,r", "read data from raw RGE files instead of SDF files");

  if (species)
    option_desc.add_options()
      ("species,s", po::value<std::string>(&speciesName),"name of the data block containing the species ids (default: 'Species')");

  if (mesh)
    option_desc.add_options()
     ("mesh", po::value<std::string>(&meshName),"name of the data block containing the species coordinates (default: 'Particles')");

  if (momentum)
    option_desc.add_options()
      ("px", po::value<std::string>(&pxName),"name of the data block containing the species x momentum (default: 'Px')")
      ("py", po::value<std::string>(&pyName),"name of the data block containing the species y momentum (default: 'Py')")
      ("pz", po::value<std::string>(&pzName),"name of the data block containing the species z momentum (default: 'Pz')");

  if (weight)
    option_desc.add_options()
      ("weight", po::value<std::string>(&weightName),"name of the data block containing the species weights (default: 'Weight')");
}

pParticleStream ParticleStreamFactory::getParticleStream(po::variables_map &vm)
{
  if (vm.count("input")<1) return pParticleStream();
  if (vm.count("species")<1) speciesName = "Species";
  if (vm.count("px")<1) pxName = "Px";
  if (vm.count("py")<1) pyName = "Py";
  if (vm.count("pz")<1) pzName = "Pz";
  if (vm.count("mesh")<1) meshName = "Particles";
  if (vm.count("weight")<1) weightName = "Weight";
  if (vm.count("chunk")<1) chunkLength = 1024*1024;

  pParticleStream pstream;

  if (vm.count("raw")<1)
  {
    std::cerr << "Making SDF stream!\n";
    pSdfFile file(new SdfFile(inputName));
    SdfParticleStream *sdfStream = new SdfParticleStream(file, chunkLength);
    if (species) sdfStream->addSpecies(speciesName);
    if (momentum)
    {
      sdfStream->addPx(pxName);
      sdfStream->addPy(pyName);
      sdfStream->addPz(pzName);
    }
    if (mesh) sdfStream->addMesh(meshName);
    if (weight) sdfStream->addWeight(weightName);

    pstream = pParticleStream(sdfStream);
  }
  else
  {
    pstream = pParticleStream(new RawParticleStream(inputName, chunkLength));
  }

  return pstream;
}
