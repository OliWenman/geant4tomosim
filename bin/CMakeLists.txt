# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties( ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/src/sim.pyx PROPERTIES CYTHON_IS_CXX TRUE )

#Creates an exectuable for the python code that has wrapped the C++ code
cython_add_standalone_executable( X-Ray_Simulation MAIN_MODULE run.py
  				  ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/src/sim.pyx
  				  run.py
  				  ${CYTHON_CMAKE_EXAMPLE_SOURCE_DIR}/src/Simulation.cc
				  ${sources})

#Link any external libraires to the executable
target_link_libraries(X-Ray_Simulation ${Geant4_LIBRARIES})

