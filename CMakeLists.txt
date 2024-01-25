cmake_minimum_required(VERSION 3.12)
cmake_policy(VERSION 3.12)

# set the project name
project(msdf)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MPI_CXX_SKIP_MPICXX TRUE)

find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)
find_package(Schnek REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# add the executable
add_executable(msdf
    src/commands.cpp
    src/angular.cpp
    src/dataio.cpp
    src/distfunc.cpp
    src/hdfstream.cpp
    src/ls.cpp
    src/msdf.cpp
    src/particlestream.cpp
    src/pcount.cpp
    src/penergy.cpp
    src/phaseplot.cpp
    src/screen.cpp
    src/sdfblock.cpp
    src/sdfdatatypes.cpp
    src/tohdf.cpp 
    src/common/sdffile.cpp
    src/common/sdfheader.cpp
    src/common/sdfio.cpp
)

function(setoptions target)
    target_include_directories(${target} PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories(${target} PUBLIC ${MPI_INCLUDE_PATH})
    target_include_directories(${target} PUBLIC ${HDF5_INCLUDE_DIRS})

    target_link_libraries(${target} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
    target_link_libraries(${target} ${HDF5_LIBRARIES})
    target_link_libraries(${target} schnek)
    target_link_libraries(${target} Boost::program_options Boost::system Boost::filesystem)
endfunction()

setoptions(msdf)
