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
  - $ module load python/anaconda-savu 
  - $ module load geant4/10.04
  - $ cmake /path/to/source/code/
  - $ make -jN
  
  Where N is the number of cores on your machine.
  
  # How to use
In the script folder is where the macro files are held. For this we only need to adjust the Geometry.mac and pySettings.mac. The pySettings one contains all other properties and values to do with the simulation such as the physics processes involved, and seed used. The Geometry.mac one should contain commands to construct the sample(s) you want to simulate.
In the Bin directory is where the run.py python script is loacted. To change variables for the simulation to do with projections you would get is controlled from python such as the energies of the beam, the rotation angle, the number of projections, etc. Once you have adjusted everything to your needs, you can then run the simulation by doing the following:

  - $ python run.py /path/to/save/data/fileName.nxs
 
 If no arguments are supplied, it will save the data in the default place: ./../Output/HDF5/SimulationData.nxs


