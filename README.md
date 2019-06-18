# What is Geant4TomoSim?
It utilizes the C++ toolkit Geant4 that simulates particles through matter for tomography simulations. A user can easily setup their experiment via macro files and run it from Python. 

# Required dependcies
- Geant4 10.04
- Geant4 mpi
- xraylib 3.30
- GCC Compiler 8.0 or higher
- Cmake 2.7 or higher
- Cython 0.19.1
- Python 2.7.1.5
- Numpy
- h5py

# Build instructions
Download the code in the directoy you want
- $ cd path/to/source/code
- $ git clone link
  
Once downloaded, you can then create a build direcotry in a place of your choosing and build the code using cmake.
- $ mkdir build
- $ cd build
- $ module load python/3.7
- $ cmake /path/to/source/code
- $ make -jN
  
Where N is the number of cores on your machine that you want to use. For example make -j4. 

Once complete, your build directory should contain the folders bin, scripts, output and src.

# Functions
The G4TomoSim package has functions to be used in a python script to run and setup your simulation.

'''python
G4TomoSim.execute_macrolist(list[string] macrolist)
'''

# How to use
The scripts directory is where the macro files are held. These .mac files control what conditions the simulation will use and can be edited to change the inputs of the commands that are supplied within the macro files. The Settings.mac contains all properties and values to do with the simulation such as the physics processes involved, and seed used. The Geometry.mac one contains commands to construct the sample(s) you want to simulate such as the shape and material it is made of.

In the bin directory is where the run.py python script is loacted. To change variables for the simulation to do with the projections is within the run.py. This includes the energies of the beam for each image, the rotation angle, the number of projections, etc. Once you have adjusted everything to your simulations needs, you can then run the simulation by doing the following from the bin directory:

  - $ cd bin
  - $ python run.py /path/to/save/data/fileName.nxs
 
If no arguments are supplied, it will save the data in the default place: /path/to/source/code/Output/HDF5/SimulationData.nxs

Once the simulation is finished, you can view the results of the data using DAWN and then use Savu to reconstuct your simulated data.
 
# Simulation time
The simulation time for a single projection depends on a variety of factors such as the geometry and materials of your sample, physics processes used, energy of the beam, the number of particles you use per projection and CPU used. A good guide though is it can range from 20 seconds to a minute using 10,000,000 particles per image depending on the factors explained above running on a Intel(R) Xeon(R) W-2123 CPU @ 3.60GHz. The more photons used the better projection will be, about 1,000 particles+ per pixel will yield a good enough image. However, the more pixels you use, the more particles will have to be used to keep the same ratio of particles/pixels. So it will be a trade off between resolution, exposure and simulation time.   


