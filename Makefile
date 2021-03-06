
TARGET=msdf

OFLAGS  = -g -O0 -Wall
#OFLAGS  = -O3 -Wall
#OFLAGS  = -O3 -Wall -DNDEBUG

INCLUDE = -I/usr/local/include -I/usr/include/hdf5/mpich
#CXX     = $(X_CXX)
CXX     = mpic++

CXXFLAGS = $(OFLAGS)

SOURCES = \
  src/commands.cpp \
  src/angular.cpp \
  src/dataio.cpp \
  src/distfunc.cpp \
  src/hdfstream.cpp \
  src/ls.cpp \
  src/msdf.cpp \
  src/particlestream.cpp \
  src/pcount.cpp \
  src/penergy.cpp \
  src/phaseplot.cpp \
  src/screen.cpp \
  src/sdfblock.cpp \
  src/sdfdatatypes.cpp \
  src/sdffile.cpp \
  src/sdfheader.cpp \
  src/tohdf.cpp \


OBJECTS = $(SOURCES:.cpp=.o)

LDFLAGS = -Wl,-rpath=/usr/lib/x86_64-linux-gnu/hdf5/openmpi -L/usr/lib/x86_64-linux-gnu/hdf5/openmpi

LOADLIBS = \
  -lhdf5_hl \
  -lhdf5 \
  -lm \
  -lboost_program_options \
  -lboost_filesystem \
  -lboost_regex \
  -lboost_system
  
BINDIR = bin
OBJDIR = obj

FULLTARGET = $(BINDIR)/$(TARGET)

all: $(FULLTARGET)

$(FULLTARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	$(CXX) $^ -o $@ $(OFLAGS) $(LDFLAGS) $(LOADLIBS)


%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $(INCLUDE) $<


clean:
	-rm -f $(OBJECTS) core $(FULLTARGET)


