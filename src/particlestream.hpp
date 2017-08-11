/*
 * particlestream.h
 *
 *  Created on: 19 Oct 2010
 *      Author: Holger Schmitz
 *       Email: h.schmitz@imperial.ac.uk
 */

#ifndef PARTICLESTREAM_H_
#define PARTICLESTREAM_H_

#include "cfddatatypes.hpp"
#include "cfdfile.hpp"
#include "sdfdatatypes.hpp"
#include "sdffile.hpp"
#include <fstream>
#include <map>
#include <boost/program_options.hpp>

class ParticleStream
{
  public:
    virtual bool eos()=0;
    virtual void getNextChunks()=0;
    virtual bool isRaw() = 0;

  public: // Dirty programming style, but we don't mind.
    pDataGrid2d mesh;
    pDataGrid1d species;
    pDataGrid1d px;
    pDataGrid1d py;
    pDataGrid1d pz;
    pDataGrid1d weight;
    virtual int getRank() = 0;
};
typedef boost::shared_ptr<ParticleStream> pParticleStream;

class CfdParticleStream : public ParticleStream
{
  public:
    CfdParticleStream(pCfdFile file_, int64_t chunkLength_)
        : file(file_),
          chunkLength(chunkLength_)
    {}

    void addMesh(std::string blockname);
    void addSpecies(std::string blockname);
    void addWeight(std::string blockname);
    void addPx(std::string blockname);
    void addPy(std::string blockname);
    void addPz(std::string blockname);

    bool eos();
    void getNextChunks();
    bool isRaw() { return false; }
    int getRank() { return meshStream->getRank(); }
  private:
    pCfdMeshVariableStream getValidVarStream();

    pCfdFile file;
    int64_t chunkLength;

    pCfdMeshStream meshStream;
    pCfdMeshVariableStream speciesStream;
    pCfdMeshVariableStream weightStream;
    pCfdMeshVariableStream pxStream;
    pCfdMeshVariableStream pyStream;
    pCfdMeshVariableStream pzStream;
};

class SdfParticleStream : public ParticleStream
{
  private:
    pSdfMeshVariableStream getValidVarStream();

    pSdfFile file;
    int64_t chunkLength;

    pSdfMeshStream meshStream;
    pSdfMeshVariableStream weightStream;
    pSdfMeshVariableStream pxStream;
    pSdfMeshVariableStream pyStream;
    pSdfMeshVariableStream pzStream;
  public:
    SdfParticleStream(pSdfFile file_, int64_t chunkLength_)
        : file(file_),
          chunkLength(chunkLength_)
    {}

    void addMesh(std::string blockname);
    void addSpecies(std::string blockname);
    void addWeight(std::string blockname);
    void addPx(std::string blockname);
    void addPy(std::string blockname);
    void addPz(std::string blockname);

    bool eos();
    void getNextChunks();
    bool isRaw() { return false; }
    int getRank() { return meshStream->getRank(); }
};

class RawParticleStream : public ParticleStream, public BinaryIo
{
  public:
    RawParticleStream(std::string file_, int64_t dataLength_);
    bool eos();
    void getNextChunks();
    bool isRaw() { return true; }
    int getRank() { return 2; }
  private:
    void readHeader();
    void readSpeciesHeader();
    void readChunkHeader();
    void advanceChunk();
    void initStream();
    void openStream();
    void advanceStream();

    std::string file;
    int64_t dataLength;

    std::ifstream stream;

    int32_t count_i, rank, n_species, chunk;
    double x_min, x_max, y_min, y_max;
    int32_t npleft;
    double spcharge, spmass;

    int32_t chunk_size;
    int32_t chunk_remainder;

    int32_t species_count;

    schnek::Grid<float, 1, MsdfGridChecker> buffer;
    DataGrid1d speciesBuffer;
    bool end_reached;

    std::map<int, std::string> filenames;
    std::map<int, std::string>::iterator fname_iter;

};

class ParticleStreamFactory
{
  private:
    bool species;
    bool momentum;
    bool mesh;
    bool weight;

    std::string inputName;
    std::string outputName;

    std::string speciesName;
    std::string meshName;
    std::string weightName;

    std::string pxName;
    std::string pyName;
    std::string pzName;

    int64_t chunkLength;
  public:
    ParticleStreamFactory()
      : species(false), momentum(false), mesh(false), weight(false) {}
    void setProgramOptions(boost::program_options::options_description &option_desc);
    pParticleStream getParticleStream(boost::program_options::variables_map &vm);

    ParticleStreamFactory& addSpecies() { species = true; return *this; }
    ParticleStreamFactory& addMesh() { mesh = true; return *this; }
    ParticleStreamFactory& addMomentum() { momentum = true; return *this; }
    ParticleStreamFactory& addWeight() { weight = true; return *this; }
};


#endif /* PARTICLESTREAM_H_ */
