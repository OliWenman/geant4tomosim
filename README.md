# geant4tomosim
Simulation of tomography data using the Geant4 toolkit. Wrapped in Cython to be called from Python.

# Required dependcies
- Geant4/10.04
- GCC Compiler 8.0 or higher
- Cmake 2.7 or higher
- Cython 0.19.1
- Python 2.7.1.5
- h5py

# Build instructions
- Gitclone code 
- In the terminal where the code is saved:
  - $ mkdir build
  - $ cd build
  - $ module load geant4/10.04
  - $ cmake /path/to/source/code/
  - $ make

