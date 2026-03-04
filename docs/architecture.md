# msdf Architecture

## Purpose and Scope

`msdf` is a C++17 command-line tool for inspecting and transforming data from SDF files produced by EPOCH-style PIC workflows. It is not a full generic SDF implementation: the code focuses on selected block types and analysis tasks used by existing commands.

The SDF format reference in `docs/information.tex` describes many block/data variants; the current implementation reads a practical subset, primarily mesh and particle variable data.

---

## High-level Structure

The executable is built as a single binary (`msdf`) with a command-dispatch architecture:

1. `main` registers command factories.
2. The first CLI argument selects a command.
3. The selected command parses its own options and runs its pipeline.

Core source layout:

- `src/msdf.cpp`: entry point and top-level exception handling.
- `src/commands.hpp`, `src/commands.cpp`: command interfaces, factories, and registration.
- `src/common/*`: low-level binary I/O, SDF file header/block header parsing, file model.
- `src/sdfblock.*`, `src/sdfdatatypes.*`: block type dispatch and typed block readers/streams.
- `src/dataio.*`: higher-level mesh data access abstraction.
- `src/particlestream.*`: chunked particle-oriented streaming over SDF (or raw) inputs.
- `src/hdfstream.*`, `src/hdfstream.t`: HDF5 output stream abstraction.
- `src/*.cpp` command implementations (`ls`, `toh5`, `pcount`, `penergy`, `phaseplot`, `screen`, `angular`, `distfunc`).

---

## Runtime Control Flow

### 1) Program entry and dispatch

- `main` in `src/msdf.cpp` creates a global `CommandMap`, calls `register_commands`, validates the command name, and instantiates the concrete command via factory.
- Command execution is wrapped with typed exception handling:
  - `BlockNotFoundException`
  - `BlockTypeUnsupportedException`
  - `GenericException`

### 2) Command-level option parsing

Each command owns a `boost::program_options` configuration and positional mapping. `execute(argc, argv)` performs parsing, validation, then launches data processing.

### 3) Data acquisition path (SDF)

Most commands follow this chain:

- Construct `SdfFile` from input path/stream.
- `SdfFileHeader` parses the file header once.
- `SdfFile` iterates `num_blocks` and builds a `SdfBlockHeaderList`.
- Command resolves specific blocks by id (`getBlockHeader(name)`).
- `SdfBlockHeader::getData(...)` or `getDataStream(...)` creates typed readers.

### 4) Data processing and sinks

- Commands either:
  - print summaries (`ls`, `pcount`, `penergy`),
  - write ASCII data (`angular`, `distfunc`, `screen`, optional `toh5 --text`),
  - or write HDF5 grids (`toh5`, `phaseplot`).

---

## Core Components

### 1. Binary and exception utilities (`src/common/binaryio.hpp`)

- `detail::readValue` and `detail::readString` provide primitive binary reads.
- Defines the project’s core exception types used across command and parsing layers.

### 2. SDF file model (`src/common/sdfheader.*`, `src/common/sdffile.*`)

- `SdfFileHeader` parses fixed header fields including:
  - version/revision,
  - first block location,
  - block header length,
  - string length,
  - number of blocks.
- `SdfFile` stores:
  - source stream,
  - parsed file header,
  - list of block headers.
- Block list construction seeks using `next_block_location` pointers.

### 3. Block metadata and type dispatch (`src/sdfblock.*`, `src/common/sdfio.*`)

- `SdfBlockHeader` parses per-block fields (`id`, `name`, `type`, `dtype`, `ndims`, offsets).
- `sdfio` maps on-disk integer codes to enums and readable names.
- Data materialization entry points:
  - `getData(...)` for fully materialized in-memory objects.
  - `getDataStream(...)` for chunked streaming objects.

Current `getData(...)` support is intentionally narrow:

- `sdf_plain_variable`
- `sdf_point_variable`

Current `getDataStream(...)` support:

- `sdf_point_variable` via `SdfMeshVariableStream`

Unsupported block types throw `BlockTypeUnsupportedException`.

### 4. Typed SDF readers and streamers (`src/sdfdatatypes.*`)

### `SdfMeshVariable`

- Reads metadata (`mult`, `units`, `meshId`, dims/stagger/np depending on block kind).
- Supports real4 and real8 payloads.
- Materializes into Schnek grids:
  - `DataGrid1d`, `DataGrid2d`, `DataGrid3d`.
- Handles:
  - plain mesh-like variable arrays,
  - point variable particle arrays,
  - lagrangian mesh variant paths.

### Streaming classes

- `SdfMeshVariableStream`: chunked 1D value stream (used for particle attributes like species/momentum/weight).
- `SdfMeshStream`: chunked coordinate stream for particle mesh blocks (`rank × np` style reading).

These classes enable bounded-memory processing of large particle datasets.

### 5. Higher-level data facade (`src/dataio.*`)

- `MeshDataImpl` interface abstracts data source.
- `SdfMeshDataImpl` binds `SdfFile` + `SdfMeshVariable` for block extraction.
- `MeshData` wraps CLI option wiring + validation + accessors.
- `MultiMeshData` exists for multi-input patterns (used by `joinslices` scaffolding).

### 6. Particle streaming facade (`src/particlestream.*`)

`ParticleStream` normalizes iterative access to particle chunks:

- `mesh` (positions),
- `species`,
- `px`, `py`, `pz`,
- `weight`.

Implementations:

- `SdfParticleStream`: builds stream objects per named SDF blocks and advances all streams in lockstep per chunk.
- `RawParticleStream`: legacy/raw multi-file format reader (`*.NNN`) with internal chunk/species headers.

`ParticleStreamFactory` configures and builds either implementation from CLI options.

### 7. HDF5 output layer (`src/hdfstream.*`, `src/hdfstream.t`)

- `HDFstream` is a thin base wrapper around HDF5 file handles and block naming.
- `HDFostream` and `HDFistream` provide typed grid operators.
- Used by conversion/analysis commands to write result datasets.

---

## Command Architecture

All commands implement `MsdfCommand` (`execute`, `print_help`) and are created via `MsdfCommandFactory` entries.

Registered commands in `register_commands`:

- `ls`: list block ids/types/offsets from SDF metadata.
- `toh5`: export one SDF variable block to HDF5 or text.
- `pcount`: count particles per species.
- `penergy`: compute species thermal moments/temperatures.
- `phaseplot`: 2D weighted phase-space histograms (HDF5 output).
- `screen`: projected particle distribution at a screen position (ASCII).
- `angular`: angular distribution output (ASCII).
- `distfunc`: 1D integrated distribution functions (ASCII).

Note: `joinslices` is implemented (`src/commands/joinslices.*`) and has a factory, but is currently not added in `register_commands`, so it is not reachable from the CLI at runtime.

---

## Data and Memory Model

- Primary numeric containers are Schnek `Grid<double, N>` types (`N=1..3`).
- Full-grid commands (`toh5`) materialize complete arrays.
- Particle analysis commands use chunked streaming to cap memory footprint.
- Precision conversion paths support float/double payloads; unsupported dtypes fail fast.

---

## Build and Dependencies

From `CMakeLists.txt`:

- Language: C++17.
- Required packages:
  - MPI,
  - HDF5,
  - Boost (`program_options`, `system`, `filesystem`),
  - Schnek.
- Single executable target: `msdf`.

Although MPI is linked, command pipelines are currently file-centric and mostly structured as serial processing logic.

---

## Tests

The `test/` tree currently provides lightweight unit tests:

- command registration sanity (`test/commands.cpp`),
- binary utility and exception behavior (`test/common/binaryio_spec.cpp`).

There is currently limited automated coverage for end-to-end SDF block parsing and command outputs.

---

## Architectural Characteristics and Current Constraints

- Layered flow: **CLI command layer** → **SDF parsing/streaming layer** → **analysis/transformation** → **output sinks**.
- Strong use of runtime polymorphism (`MsdfCommand`, `MeshDataImpl`, `ParticleStream`).
- Practical SDF subset support rather than full spec coverage from `information.tex`.
- Stream-based particle path is the main scalability mechanism for large files.
- Some historical naming remains (`mcfd` strings in help text/comments), but functionality is centered on SDF workflows.

This architecture is effective for adding new commands: most extensions can reuse `SdfFile` + `SdfBlockHeader` resolution and either `MeshData` (materialized arrays) or `ParticleStreamFactory` (chunked particle scans).
