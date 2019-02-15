# What is Geant4TomoSim?
The simulation of tomography data using the C++ Geant4 toolkit. Wrapped in Python via Cython. Saves its data as a NeXus file (.nxs) and uses the NXtomo format. 

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
  
- Once downloaded, you can then create a build direcotry in a place of your choosing and build the code using cmake.
  - $ mkdir build
  - $ cd build
  - $ module load python/anaconda-savu 
  - $ module load geant4/10.04
  - $ cmake /path/to/source/code
  - $ make -jN
  
  Where N is the number of cores on your machine that you want to use. For example make -j4
  
# How to use
In the script directory is where the macro files are held. For this we only need to adjust the Geometry.mac and pySettings.mac. The pySettings contains all properties and values to do with the simulation such as the physics processes involved, and seed used. The Geometry.mac one should contain commands to construct the sample(s) you want to simulate.
In the Bin directory is where the run.py python script is loacted. To change variables for the simulation to do with the projections is within the run.py. This includes the energies of the beam for each image, the rotation angle, the number of projections, etc. Once you have adjusted everything to your needs, you can then run the simulation by doing the following:

  - $ python run.py /path/to/save/data/fileName.nxs
 
If no arguments are supplied, it will save the data in the default place: /path/to/source/code/Output/HDF5/SimulationData.nxs
 
# Simulation time
The simulation time for a single projection depends on a variety of factors such as the geometry and materials of your sample, physics processes used, energy of the beam, the number of particles you use per projection and CPU used. A good guide though is it can range from 20 seconds to a minute using 10,000,000 particles per image depending on the factors explained above running on a Intel(R) Xeon(R) W-2123 CPU @ 3.60GHz. The more photons used the better projection will be, about 1,000 particles+ per pixel will yield a good enough image. However, the more pixels you use, the more particles will have to be used to keep the same ratio of particles/pixels. So it will be a trade off between resolution, exposure and simulation time.   


