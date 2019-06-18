# Geant4TomoSim
Geant4TomoSim is a python package developed for the use of easily writing python scripts to simulate tomography data. It utilizes the C++ toolkit Geant4 that simulates particles through matter. Geant4 uses a monte-carlo approach and allows for unique physics to be simulated such as scattering, fluorescence and refraction. A tomography experiment is built using this toolkit and wrapped as a class and wrapped via Cython, making it easily customizable for a user to control and setup their tomography experiemnts in a python script.

# Required dependcies
- RHEL 7
- Geant4 10.04
- xraylib 3.30
- GCC Compiler 7.0 or higher
- CMake 2.7 or higher
- Cython 0.19.1
- Python 3.7
- Numpy
- h5py
- mpi4py

# Build instructions
In the command prompt, git clone in the directoy of your choosing.
```bash
git clone <link>
```
Once downloaded, you can then create a build direcotry in a place of your choosing and build the code using cmake. In the command prompt, do the following:
```bash
mkdir build
cd build
module load python/3.7
cmake /path/to/source/code
make -jN
```
Where N is the number of cores on your machine that you want to use. For example make -j4. 

Once complete, your build directory should contain the folders bin, settings, output and src.

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

## Get functions
### Absorption detector
```python
#Get the detector data after the simulation has finished 
G4TomoSim.absorptiondetector_getprojection() 

#Get information about the detector
G4TomoSim.absorptiondetector_getxpixels()
G4TomoSim.abosrptiondetector_getypixels()
G4TomoSim.absorptiondetector_getdimensions()
```

### Fluorescence detector
```python
#Get the detector data after the simulation has finished
G4TomoSim.fluorescencedetector_getfullmappingdata() #3 dimensional
G4TomoSim.fluorescencedetector_getfullfielddata()   #1 dimenionsal
G4TomoSim.fluorescencedetector_getenergybins()      #1 dimensional

#Get information about the detector
G4TomoSim.fluorescencedetector_getnoenergybins()
G4TomoSim.fluorescencedetector_getposition()
G4TomoSim.fluorescencedetector_getdimensions()

G4TomoSim.fluorescencedetector_fullmappingactive()
G4TomoSim.fluorescencedetector_fullfieldactive()
```
The fluorescence detector data is optional. Therefore the data collected needs to be actived via commands before the simulation starts to simulate the fluorescence data and access it in python. 
```
/detector/fluorescence/fullmapping True
/detector/fluorescence/fullfield   True
```

### Beam data
```python
#Get the beam energy used to plot a beam profile for the simulation
G4TomoSim.beam_getenergybins() #1 dimensional
G4TomoSim.beam_getintesity()   #1 dimensional

G4TomoSim.beam_getnobins() #Number of bins
```
# Macro files and commands
Macro files contain a list of commands that Geant4 uses to easily control the setup of a simulation. For guidance, help and a full list of Geant4 pre-built commands, please visit http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Control/AllResources/Control/UIcommands/_.html.

Custom commands I've built using the Geant4 toolkit to control the G4TomoSim package are below:

## Detectors
### Absorption
```
/detector/absorption/halfdimensions <x_dim> <y_dim> <z_dim> <unit>
/detector/absorption/xpixels        <no_pixels>
/detector/absorption/ypixels        <no_pixels>
```
### Fluorescence
```
/detector/fluorescence/halfdimensions <x_dim> <y_dim> <z_dim> <unit>
/detector/fluorescence/fullmapping    <bool>
/detector/fluorescence/fullfield      <bool>
/detector/fluorescence/bins           <no_bins>
/detector/fluorescence/maxenergy      <energy> <unit>
```
## World
```
/world/halfdimensions <x_dim> <y_dim> <z_dim> <unit>
/world/material       <material>
```
## Beam
```
/beam/bins      <no_bins>
/beam/maxenergy <energy> <unit>

/beam/energy/mono <energy> <unit>
/beam/particle    <particle>

# Position commands
/beam/pos/auto   <bool>
/beam/momentum   <x_mom> <y_mom> <z_mom> 
/beam/pos/centre <x_cen> <y_cen> <z_cen> <unit>
/beam/pos/halfx  <x_dim> <unit>
/beam/pos/halfy  <y_dim> <unit>
```
Beam commands uses the Geant4 G4ParticleGun classs which is fast and basic. To use a more advanced particle gun, G4GeneralParticleSource, with more functionality, options and pre-built commands, use:
```
/beam/gps <bool>
```
Set it to True. All /beam/ commands will also affect gps commands. For instance "/beam/pos/halfx" is equivalent to "/gps/pos/halfx" if "/beam/gps True". 

For a list of gps commands and guidance, please visit:
 - http://www.apc.univ-paris7.fr/~franco/g4doxy/html/classG4GeneralParticleSourceMessenger.html#369e77c64ee8281a4c20bbcad87f476e
 - http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html
 - http://hurel.hanyang.ac.kr/Geant4/Geant4_GPS/reat.space.qinetiq.com/gps/examples/examples.html

Warning: there is a known issue that when using gps, the program will end with a segmentation fault with G4TomoSim.

## Simulation
```
/simulation/seed     <int>
/simulation/graphics <bool>	
```

## Physics
Control the physics processes used for the simulation.
```python
/physics/gamma/livermore/photoelectric      <bool>
/physics/gamma/livermore/comptomscattering  <bool>
/physics/gamma/livermore/raylieghscattering <bool>
/physics/gamma/fluorescence                 <bool>
/physics/gamma/refraction                   <bool>

/physics/opticalphoton/refraction <bool>
/physics/opticalphoton/absorption <bool>
```
Note: Refraction physics is designed to be used with opticalphoton's and not gamma (Geant4 treats waves and particles properties seperate with opticalphotons and gamma particles respectively). Therefore one side effect is that fluorescence doesn't appear to work when refraction is on. Needs more testing.
## Materials
Create new custom materials for your sample or world material.

### Create a new element
```
/materials/define/element <new_element> <z> <molecule_mass> <unit> <density> <unit>
```
### Create a new isotope
```
/materials/define/isotope     <new_isotope> <z> <no_necleans> <atomic_weight> <unit>
/materials/define/isotope_mix <new_isotope_mix> <symbol> <no_isotopes>
/materials/addto/isotope_mix  <new_isotope> <element> <abundance> <unit>
/materials/addto/add_desnity  <new_isotope> <density> <unit> 
```

### Create a new molecule
```
/materials/define/molecule <new_molecule> <no_atoms> <density> <unit> 
/materials/addto/molecule  <new_molecule> <element> <n_atoms>
```
### Create a new compound
```
/materials/define/compound <new_material> <no_components> <density> <unit> 
/materials/addto/compound  <new_material> <element> <fractional_mass> <unit>
```
### Create a new mixture
```
/materials/define/mixture  <new_mixture> <no_components> <density> <unit>
/materials/addto/mixture   <new_mixture> <material> <fractional_mass> <unit>
```
### MaterialsPropertyTable
```
/material/mpt/print <material>
```
#### Add optical properties
All <_array> values use the notation of {n1,n2,n3,...}[unit]. For <energy_arrays>, it is possible to use the equivalent python numpy function linspace to easily create a large array. For instance, <energy_array> = linspace(start,end,steps)[unit].
```
/material/mpt/xraylib/add/allopticalproperties <material> <energy_array>
/material/mpt/xraylib/add/refractive_index     <material> <energy_array>
/material/mpt/xraylib/add/absorption           <material> <energy_array>
/material/mpt/add/refractive_index             <material> <energy_array> <refractive_index_array>
/material/mpt/add/complexrefractive_index      <material> <energy_array> <complexrefractive_index_array>
/material/mpt/add/absorptionlength             <material> <energy_array> <absorptionlength_array>
```
## Sample
Build your own custom samples using the below commands. 
### G4VSolids
#### Basic solids
```
/sample/G4VSolid/box       <name> <x_dimension> <y_dimension> <z_dimension> <unit>
/sample/G4VSolid/sphere    <name> <inner_r> <outer_r> <unit> <start_phi> <end_phi> <start_theta> <end_theta> <unit>
/sample/G4VSolid/cylinder  <name> <inner_r> <outer_r> <height> <unit> <start_phi> <end_phi> <unit>
/sample/G4VSolid/ellipsoid <name> <px_semi_axis> <py_sami_axis> <pz_semi_axis> <pz_bottomcut> <pz_topcut> <unit>
/sample/G4VSolid/trapezoid <name> <dx1> <dx2> <dy1> <dy2> <dx> <unit>
```
#### Boolean solid
Create more complicated shapes by joining or subtracting solids away from each other. The insideposition and insiderotation determines how and where the solids are connected/subtracted relative to each other.
```
/sample/G4VSolid/subtract       <name> <component_name1> <component_name2> 
/sample/G4VSolid/union          <name> <component_name1> <component_name2>
/sample/G4VSolid/insideposition <name> <x_pos> <y_pos> <z_pos> <unit>
/sample/G4VSolid/insiderotation <name> <x_rot> <y_rot> <z_rot> <unit>
```
### G4LogicalVolumes
A sample is not automatically placed in the world volume unless is has a material assigned to it (placing it at position 0,0,0)
```
/sample/G4LogicalVolume/material <name> <material>
/sample/G4LogicalVolume/colour   <name> <colour>
```
### G4PVPlacements
Alter the samples starting position, rotation and how it rotates throughout a tomography scan.
```
/sample/G4VPhysicalVolume/position <name> <x_pos> <y_pos> <z_pos> <unit>
/sample/G4VPhysicalVolume/rotation <name> <x_rot> <y_pos> <z_pos> <unit>

/sample/G4VPhysicalVolume/tilt/anglex <x_rot> <unit>
/sample/G4VPhysicalVolume/tilt/angley <y_rot> <unit>
/sample/G4VPhysicalVolume/tilt/centre <x_centre> <y_centre> <z_zentre>

/sample/checkforoverlaps <bool>
```
# Examples
In the bin directory are two python scripts. Both scripts simuation variables are controlled via the settings folder. "run.py" is a python script that uses the G4TomoSim.simulatetomography function and automatically saves the data for you. To run the code, simply do in the teriminal (assuming in the build directoy): 
```bash
python ./bin/run.py [filepath]
```
Where [filepath] is an optinal argument to save the data in a location of your choosing. If no argument supplied, it will save the data in the output folder.

The second python script is "mpi_run.py" and is an example on how to utilise mpi4py to parallise the simulation. To run, simply do:
```bash
mpiexec -n <no_cores> python ./bin/mpi_run.py [filepath]
```


