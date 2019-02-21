# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/src/sim.pyx
  			    ${Geant4_LIBRARIES}
  			    ${pkgcfg_lib_LIBXRL_xrl}
  			    PROPERTIES CYTHON_IS_CXX TRUE )

# Creates the .so file and uses all the other files needed with it using "sources"
cython_add_module( sim sim.pyx Simulation.cc ${sources})

#Link any external libraries used with .so file
target_include_directories(sim PUBLIC ${LIBXRL_INCLUDE_DIRS})
target_link_libraries( sim ${Geant4_LIBRARIES} ${pkgcfg_lib_LIBXRL_xrl})

file(COPY G4Units.py DESTINATION ./)

