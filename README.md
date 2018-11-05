# geant4tomosim
Simulation of data for tomography using GEANT4.

# Required
- Geant4/10.04
- Cmake 2.7 or higher

# Build instructions
- Download code and put in a source folder e.g /user/home/documents/geant4/sourcefile
- Make a build directory, not in the same location as the source folder e.g /user/home/documents/geant4/build
- In the terminal "module load geant4/10.04" 
- Type in terminal "cmake -DCMAKE_CXX_COMPILER=/dls_sw/apps/gcc/8.1.0/bin/g++ -DCMAKE_C_COMPILER=/dls_sw/apps/gcc/8.1.0/bin/gcc -DGeant4_DIR=/dls_sw/apps/Geant4/geant4.10.04/geant4.10.04-install/lib64/Geant4-10.4.2/ path/to/source/file" 
- "make -j2"


