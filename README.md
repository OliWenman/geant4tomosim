# geant4tomosim
Simulation of tomography data using the C++ Geant4 toolkit. Wrapped in Python via Cython.

# Required dependcies
- Geant4/10.04
- GCC Compiler 8.0 or higher
- Cmake 2.7 or higher
- Cython 0.19.1
- Python 2.7.1.5
- h5py

# Build instructions
- Download the code in the directoy you want
  - $ cd path/to/source/code
  - $ git clone link
  
- Once downloaded, you can then create a build direcotry of your choosing and build the code using cmake
  - $ mkdir build
  - $ cd build
  - $ module load python/anaconda-savu 
  - $ module load geant4/10.04
  - $ cmake /path/to/source/code/
  - $ make -jN
  
  Where N is the number of cores on your machine.
  
# How to use
In the script directory is where the macro files are held. For this we only need to adjust the Geometry.mac and pySettings.mac. The pySettings one contains all properties and values to do with the simulation such as the physics processes involved, and seed used. The Geometry.mac one should contain commands to construct the sample(s) you want to simulate.
In the Bin directory is where the run.py python script is loacted. To change variables for the simulation to do with the projections is within the run.py. This includes the energies of the beam for each image, the rotation angle, the number of projections, etc. Once you have adjusted everything to your needs, you can then run the simulation by doing the following:

  - $ python run.py /path/to/save/data/fileName.nxs
 
 If no arguments are supplied, it will save the data in the default place: /path/to/source/code/Output/HDF5/SimulationData.nxs


