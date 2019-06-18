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
The G4TomoSim package has functions to be used in a python script to run and setup your simulation. To use is as follows:

## Setup
```python
g4tomosim.G4TomoSim(int verbose = 1, bool interactive = False)
```
Create an instance of the class. Verbose is used to determine how much output there is to the terminal (use 0 if using mpi) and interactive is used to re-eneter a command if there is an error.


```python
G4TomoSim.execute_macrolist(list[str] macrolist)
```
Pass a list of macro files to be executed to control the setup of the simulation.


```python
G4TomoSim.execute_macro(str macro)
```
Execute a single macro file to control the setup of the simulation.


```python
G4TomoSim.execute_command(str command)
```
Execute a command to control the setup of the simulation.

```python
G4TomoSim.setup_visualization(str path, str filename)
```
Output a .heprep file to later view the geometry and events for your simulation in HepRApp. WARNING: don't use with more than 500 pixels for your absorption detector to visualize or use more than 1,000 particles as will crash HepRApp.

```python
G4TomoSim.set_seed(long int seed)
```
Set the seed to be used for the simulation. Useful when setting the seed different when using mpi for each rank. Can also be used via command "/simulation/seed 123456" for instance. If seed is equal to 0, a random seed is chosen.

```python
G4TomoSim.log_setup(str filepath, 
                    long long int n_particles, 
                    int n_projections, 
                    n_darkflatfields)
```
Log the setup of your simulation to a file. Copies all commands used and its associated macro file to a file and terminal (if verbose greater than 0).

## Running a simulation
```python
G4TomoSim.simulateprojection(long long int n_particles,
                             bool   flatfield,
                             double rotation_angle,
                             double zposition,
                             bool   resetdata = True)
```
Start the simulation by specifying the number of particles you want. To achieve a good projection with reduced noise, you need a high number of n_particles per absorptiondetector_pixel. Around 4,000 to 8,000 particles per pixel will be good, though the higher the better. Specify if this projection is a flat field with the flatfield parameter. If true, the sample being simulated will be not be placed in the world. For a helical scan, zposition is used to shift the sample to the correct position. resetdata is used to reset the detector array data back to 0 if true. If False, it will continue to write data on top of the data array stored in the detectors. This can be useful if you want to add breaks inbetween the simulate to check its progress or simulate image blurring. The data returned for after two different angles for example will be written on top on top of each other.

Use this command when wrapping the package in your own python scrpit, using mpi for example.

```python
G4TomoSim.simulatetomography(str           filepath,
                             long long int n_particles
                             int           n_darkflatfields,
                             numpy.ndarray rotation_angle,
                             numpy.ndarray zpositions = None)
```
Similiar to the function above but will automatically save the data for you as a NeXus file and will do tomography for you. Just specify a filepath to save the data and a numpy.ndarray for rotation_angles and zpositions for your sample. 

Not compatible with mpi, use simulateprojection and write your own script to make use of that.


# How to use
The scripts directory is where the macro files are held. These .mac files control what conditions the simulation will use and can be edited to change the inputs of the commands that are supplied within the macro files. The Settings.mac contains all properties and values to do with the simulation such as the physics processes involved, and seed used. The Geometry.mac one contains commands to construct the sample(s) you want to simulate such as the shape and material it is made of.

In the bin directory is where the run.py python script is loacted. To change variables for the simulation to do with the projections is within the run.py. This includes the energies of the beam for each image, the rotation angle, the number of projections, etc. Once you have adjusted everything to your simulations needs, you can then run the simulation by doing the following from the bin directory:

  - $ cd bin
  - $ python run.py /path/to/save/data/fileName.nxs
 
If no arguments are supplied, it will save the data in the default place: /path/to/source/code/Output/HDF5/SimulationData.nxs

Once the simulation is finished, you can view the results of the data using DAWN and then use Savu to reconstuct your simulated data.
 
# Simulation time
The simulation time for a single projection depends on a variety of factors such as the geometry and materials of your sample, physics processes used, energy of the beam, the number of particles you use per projection and CPU used. A good guide though is it can range from 20 seconds to a minute using 10,000,000 particles per image depending on the factors explained above running on a Intel(R) Xeon(R) W-2123 CPU @ 3.60GHz. The more photons used the better projection will be, about 1,000 particles+ per pixel will yield a good enough image. However, the more pixels you use, the more particles will have to be used to keep the same ratio of particles/pixels. So it will be a trade off between resolution, exposure and simulation time.   


