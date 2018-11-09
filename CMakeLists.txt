Cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(X-Ray_build)

#---------------------------------------------------------------------------
#Choose what you want with Geant4

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
  	find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
#Include the right directories 
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#Tell where to find the folders to link
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
#Copy the Data_Output folder to the build directory
file(COPY Data_Output DESTINATION ./)

#Add the executable and link the libraries to it
add_executable(X-Ray_Simulation main.cc ${sources} ${headers})
target_link_libraries(X-Ray_Simulation ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
#Copy all scripts to the build directory, i.e. the directory in which we build. So it has all scripts it need for input

set(TEST_SCRIPTS  
    MyVis.mac
    settings.mac
    Geometry.mac
    README.txt
    ViewImages.py
    SpeedTest.cc
    pywrapper.py)

foreach(_script ${TEST_SCRIPTS})
  configure_file(${PROJECT_SOURCE_DIR}/${_script}
                 ${PROJECT_BINARY_DIR}/${_script}
                 COPYONLY)
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(X-Ray_build DEPENDS X-Ray_Simulation)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS X-Ray_Simulation DESTINATION bin)


