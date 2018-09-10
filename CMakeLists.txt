# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

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
  find_package(HDF5 REQUIRED)
  #find_package(HDF5 REQUIRED COMPONENTS C CXX)
  #find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
else()
  find_package(Geant4 REQUIRED)
  find_package(HDF5 REQUIRED)
  #find_package(HDF5 REQUIRED COMPONENTS C CXX)
  #find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
endif()



#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${HDF5_INCLUDE_DIRS})



#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(COPY Data_Output DESTINATION ./)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(X-Ray_Simulation main.cc ${sources} ${headers})
link_directories(${HDF5_LIBRARY_DIRS} )
target_link_libraries(X-Ray_Simulation ${Geant4_LIBRARIES} ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} )

# target_link_libraries( readdata ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES})
#target_link_libraries(X-Ray_Simulation ${h5c++})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(TEST_SCRIPTS  
    MyVis.mac
    settings.mac
    README.txt)

foreach(_script ${TEST_SCRIPTS})
  configure_file(${PROJECT_SOURCE_DIR}/${_script}
                 ${PROJECT_BINARY_DIR}/${_script}
                 COPYONLY)
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(X-Ray_build DEPENDS X-Ray_Simulation)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS X-Ray_Simulation DESTINATION bin)


