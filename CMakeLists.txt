#Cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#project(X-Ray_build)

#set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )#
#include( UseCython )#
#---------------------------------------------------------------------------
#Choose what you want with Geant4
#option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
#if(WITH_GEANT4_UIVIS)
#	find_package(Geant4 REQUIRED ui_all vis_all)
#	if (GEANT4_FOUND)
#		message("Geant4 found, using version " ${GEANT$_VERSION_STRING})
#	else()
#		message("Geant4 was not found")
#	endif()
#	#find_package(Cython)
#else()
#  	find_package(Geant4 REQUIRED)
#	if (GEANT4_FOUND)
#		message("Geant4 found, using version " ${GEANT4_VERSION_STRING})
#	else()
#		message("Geant4 was not found")
#	endif()
#	
#	#find_package(Cython)
#endif()

#add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P#
#  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake
#  ${CMAKE_CURRENT_BINARY_DIR}
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

#----------------------------------------------------------------------------
#Include the right directories 
#include(${Geant4_USE_FILE})
#include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories( ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/include )#

#add_subdirectory( src )#
#add_subdirectory( bin )#

#Tell where to find the folders to link
#file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
#file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
#Copy the Data_Output folder to the build directory
#file(COPY Data_Output DESTINATION ./)

#Add the executable and link the libraries to it
#add_executable(X-Ray_Simulation main.cc ${sources} ${headers})
#target_link_libraries(X-Ray_Simulation ${Geant4_LIBRARIES})
#link_libraries(${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
#Copy all scripts to the build directory, i.e. the directory in which we build. So it has all scripts it need for input

#set(TEST_SCRIPTS  
#    MyVis.mac
#    settings.mac
#    Geometry.mac
#    README.txt
#    ViewImages.py
#    SpeedTest.cc
#    pywrapper.py)
#
#foreach(_script ${TEST_SCRIPTS})
#  configure_file(${PROJECT_SOURCE_DIR}/${_script}
##                ${PROJECT_BINARY_DIR}/${_script}
#                 COPYONLY)
#endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(X-Ray_build DEPENDS X-Ray_Simulation)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS X-Ray_Simulation DESTINATION bin)

#=============================================================
# Defines the CMake commands/policies
cmake_minimum_required( VERSION 2.8.5 )

# Set the project name
project( CYTHON_CMAKE_EXAMPLE )

# Make the scripts available in the 'cmake' directory available for the
# 'include()' command, 'find_package()' command.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )

# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include( UseCython )

find_package(Geant4 REQUIRED)
if(Geant4_FOUND)
  	message("-- Found Geant4 " ${Geant4_VERSION})
else()
  	message("-- Was unable to find Geant4")
endif()

# With CMake, a clean separation can be made between the source tree and the
# build tree.  When all source is compiled, as with pure C/C++, the source is
# no-longer needed in the build tree.  However, with pure *.py source, the
# source is processed directly.  To handle this, we reproduce the availability
# of the source files in the build tree.
add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P
  		   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake
  		   ${CMAKE_CURRENT_BINARY_DIR}
  		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

#Include the relevant directories for the project
include_directories( ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/include)
include(${Geant4_USE_FILE})

#Allows to include .cc files when linking
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)

# Process the CMakeLists.txt in the 'src' and 'bin' directory.
add_subdirectory( src )
add_subdirectory( bin )

#Copy the Data_Output folder to the build directory
file(COPY Output DESTINATION ./)
file(COPY scripts DESTINATION ./)
