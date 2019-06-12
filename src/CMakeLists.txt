# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/src/g4tomosim.pyx
  			                ${Geant4_LIBRARIES}
  			                #${G4mpi_LIBRARIES}
  			                ${pkgcfg_lib_LIBXRL_xrl}
  			                PROPERTIES CYTHON_IS_CXX TRUE 
  			                )

# Creates the .so file and uses all the other files needed with it using "sources"
cython_add_module(g4tomosim 
                  g4tomosim.pyx 
                  Simulation.cc 
                  ${sources}
                  )

#Link any external libraries used with .so file
target_include_directories(g4tomosim 
                           PUBLIC 
                           #${G4mpi_INCLUDE_DIR}
                           ${LIBXRL_INCLUDE_DIRS} 
                           ${NUMPY_INCLUDE_DIR} 
                           )

target_link_libraries(g4tomosim 
                      ${Geant4_LIBRARIES} 
                      #${G4mpi_LIBRARIES}
                      ${pkgcfg_lib_LIBXRL_xrl}
                      )
