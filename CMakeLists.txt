# Defines the CMake commands/policies
cmake_minimum_required( VERSION 2.8.5 )

# Set the project name
project( CYTHON_CMAKE_EXAMPLE )

# Make the scripts available in the 'cmake' directory available for the
# 'include()' command, 'find_package()' command.
set( CMAKE_MODULE_PATH 
     ${CMAKE_MODULE_PATH} 
     ${CMAKE_CURRENT_LIST_DIR}/cmake 
   )

# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include( UseCython )

#Find the Geant4 library and display it's info
find_package(Geant4 REQUIRED)

# check MPI package...
find_package(MPI )

find_package(G4mpi )

add_definitions(-DTOOLS_USE_NATIVE_MPI) 

#Find Python interpreter

#find_package(PythonInterp REQUIRED)
#if (PYTHONINTERP_FOUND)
#    message(STATUS "Python version ${PYTHON_VERSION_STRING}")
#    message(STATUS "Python_executable = ${PYTHON_EXECUTABLE}")
#else()
#    message(STATUS "ERROR: Couldn't find PythonInterpreter!") 
#endif()

#Find Python libraries
#find_package(PythonLibs REQUIRED)
#if (PYTHONLIBS_FOUND)
#    message(STATUS "Python_include_dirs = ${PYTHON_INCLUDE_DIRS}")
#    message(STATUS "Python_libraries = ${PYTHON_LIBRARIES}")
#else()
#    message(STATUS "ERROR: Couldn't find Python libraries!") 
#endif()

#Find PkgConfig. If found, find xraylib libraries
find_package(PkgConfig REQUIRED)
if (PKGCONFIG_FOUND)
    pkg_check_modules(LIBXRL REQUIRED IMPORTED_TARGET libxrl)
    #if (LIBXRL_FOUND)
    #    message(STATUS "xraylib version ${LIBXRL_VERSION}")
    #    message(STATUS "xraylib_include_dirs = ${LIBXRL_INCLUDE_DIRS}")
    #else()
    #    message(STATUS "ERROR: Couldn't find xraylib!")
    #endif()
#else()
#    message(STATUS "ERROR: Couldn't find PkgConfig")
#    message(STATUS "ERROR: Couldn't find xraylib!")
endif()

find_package(Numpy REQUIRED)

# With CMake, a clean separation can be made between the source tree and the
# build tree.  When all source is compiled, as with pure C/C++, the source is
# no-longer needed in the build tree.  However, with pure *.py source, the
# source is processed directly.  To handle this, we reproduce the availability
# of the source files in the build tree.
add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P
        		   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake
  		           ${CMAKE_CURRENT_BINARY_DIR}
  		           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})
#Include the relevant directories for the project
include_directories( ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/include)

#Allows to include .cc files when linking
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)

# Process the CMakeLists.txt in the 'src' and 'bin' directory.
add_subdirectory( src )
add_subdirectory( bin )

#Copy the Data_Output folder to the build directory
file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/output
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/settings 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
message(STATUS "Copied folders")

