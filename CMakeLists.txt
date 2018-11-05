# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#HDF5_INCLUDE_DIRS - Location of the hdf5 includes
#HDF5_INCLUDE_DIR - Location of the hdf5 includes (deprecated)
#HDF5_DEFINITIONS - Required compiler definitions for HDF5
#HDF5_C_LIBRARIES - Required libraries for the HDF5 C bindings.
#HDF5_CXX_LIBRARIES - Required libraries for the HDF5 C++ bindings
#HDF5_Fortran_LIBRARIES - Required libraries for the HDF5 Fortran bindings
#HDF5_HL_LIBRARIES - Required libraries for the HDF5 high level API
#HDF5_Fortran_HL_LIBRARIES - Required libraries for the high level Fortran bindings.
#HDF5_LIBRARIES - Required libraries for all requested bindings
#HDF5_FOUND - true if HDF5 was found on the system
#HDF5_LIBRARY_DIRS - the full set of library directories
#HDF5_IS_PARALLEL - Whether or not HDF5 was found with parallel IO support
#HDF5_C_COMPILER_EXECUTABLE - the path to the HDF5 C wrapper compiler
#HDF5_CXX_COMPILER_EXECUTABLE - the path to the HDF5 C++ wrapper compiler
#HDF5_Fortran_COMPILER_EXECUTABLE - the path to the HDF5 Fortran wrapper compiler
#HDF5_DIFF_EXECUTABLE - the path to the HDF5 dataset comparison tool

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(X-Ray_build)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  #SET(GCC_COVERAGE_LINK_FLAGS    "lhdf5")
  find_package(Geant4 REQUIRED ui_all vis_all)
  #FIND_PACKAGE(HDF5 COMPONENTS C HL NO_MODULE REQUIRED shared)
  #find_package(HDF5 REQUIRED)
  #find_package(HDF5 REQUIRED COMPONENTS C CXX)
  find_package(HDF5 REQUIRED COMPONENTS C CXX HL )
else()
  find_package(Geant4 REQUIRED)
  #FIND_PACKAGE(HDF5 COMPONENTS C HL NO_MODULE REQUIRED shared)
  #find_package(HDF5 REQUIRED)
  #find_package(HDF5 REQUIRED COMPONENTS C CXX)
  find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
endif()



#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${HDF5_INCLUDE_DIRS})

# force compilation in pre-CXX11 mode
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
#file(MAKE_DIRECTORY ${directory} Data_Output)
#file(COPY Data_Output DESTINATION ./)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(X-Ray_Simulation main.cc ${sources} ${headers})
link_directories(${HDF5_LIBRARY_DIRS} )
target_link_libraries(X-Ray_Simulation ${Geant4_LIBRARIES} ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES} ${HDF5_HL_LIBRARIES} )

#target_link_libraries( readdata ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES})
#target_link_libraries(X-Ray_Simulation ${h5c++})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(TEST_SCRIPTS  
    MyVis.mac
    settings.mac
    Geometry.mac
    README.txt
    ViewImages.py
    SpeedTest.cc)

foreach(_script ${TEST_SCRIPTS})
  configure_file(${PROJECT_SOURCE_DIR}/${_script}
                 ${PROJECT_BINARY_DIR}/${_script}
                 COPYONLY)
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(Data_Output ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${directory})
add_custom_target(X-Ray_build DEPENDS X-Ray_Simulation)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS X-Ray_Simulation DESTINATION bin)


