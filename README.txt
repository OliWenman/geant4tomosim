//////////////////////////////////////////////
//INFORMATION ON HOW TO USE THIS SIMULATION //
//////////////////////////////////////////////

INTRODUCTION
This code is used to simulate X-Ray data for tomography.

Use the "settings.mac" file to tailor the simulation to your needs. If you want visualization of the geometry of your setup, make the "data/Visualization " command to true. This will output a HepARep to be viewed in a HepARep viewer.
Do not use this visualizer with millions of objects or particles as it will take a long time to do the simulation and also crash the viewer. Recommend no more than 1,000 particles, only use it to see if simulation setup is correct.
If you want to get the exact same results each time, you can input your own seed or find what the random seed was by looking in the "SimulationSettings.txt". The seed is random if the input is 0.

Edit the commands with in the settings.mac file to get the settings you want.

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
GEOMETRY SETTINGS FOR THE TARGET

Too add objects into the geometry setup, go into the Geometry.mac file 
Create an object using Target/ObjectName/Dimensions
All length should be given its half length

Avaliable objects are:
-Cube, takes 4 areguments: length of x, y, z and units.
-HollowCube, takes 7 arguments: length of outer box for x, y and z, length of inner box for x, y and z and the unit used.
-Sphere, takes 8 arguments: length of inner radius, outer radius and unit used. Inner phi, outer phi, inner theta, outer theta and unit used(rad or deg).
-Cylinder, takes 7 arguments: length of inner radius, outer radius, height, unit used. Inner phi, outer phi, unit used(rad or deg).
-Trapezoid, takes 6 arguments: length of x on one face dx1, length of x on the opposite face dx2, length of y on one face dy2, length of y on the opposite face dy2, and its height dz and the height for the unit.
-Ellipsoid, takes 6 arguments: 
-SubtractSolid, takes 10 arugments: name of shape 1, name of shape 2, the position (x, y, z) of shape 2 inside shape 1 and its unit, and the rotation of shape 2 inside shape 1 and its unit.

Once you have entered the dimensions, assign its position, rotation and material. You must assign an object each one of these attributes, otherwise you will get an error.
A list of avaliable materials from the database can be found here: 
http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html?highlight=materials

Example:
/Target/Cylinder/Dimensions 0.1 0.15 0.4 mm 0 360 deg 
/Target/Position -0.8 -0.9 0.0 mm
/Target/Rotation 0.0 0.0 0.0 deg
/Target/Material/Database G4_Pb

For more complicated shapes, use subtract solid. First you must institate the two shapes and their dimensions used. You then must call the SubtractSolid command and name the two shapes you want to use. The name of each shape is found
by simplying putting the type of shape as well its number (starting from 0). You then simply enter its position,  rotation and material like every other shape.
Heres an example:

/Target/Cube/Dimensions 0.2 0.7 0.2 mm
/Target/Cylinder/Dimensions 0.2 0.25 0.7 mm 0 360 deg 
/Target/SubtractSolid Cylinder1 Cube0 0.0 0.0 0.0 mm 0.0 0.0 0.0 deg	
/Target/Position 0.0 0.0 0.0 mm		
/Target/Rotation 0.0 0.0 0.0 deg
/Target/Material/Database G4_Al

You can also speficy if you would like the rotation to be offset with a radius and the centre of the offset using:
/Target/OffSet/Radius 0.0 mm			
/Target/OffSet/Centre 0.0 0.0 0.0 mm

The first object you create will become the "master volume", meaning all other volumes created after will rotate around it

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
PHYSICS OPTIONS

Currently the electromagnetic processes that have been implemented are the photoelectric effect, compton scattering, rayleigh scattering and fluorescence. These processes can be model using Geant4's standard physics library or with the
Livermore Data Based models (recommened for low energy physics). Electrons will be instantly killed (in the StackingAction class) as for this simulation they are not relavent and will save computational time by not tracking them. You 
can easily toggle which physics processes you would like on like in the example below:

/physics/addPhysics/EM LivermorePhysics		#(Choose the Physics Package you want to include, refer to the README for details)
/physics/EM/PhotoElectricEffect true
/physics/EM/ComptonScattering false	
/physics/EM/RayleighScattering false
/physics/EM/Fluorescence false

All of this can be edited with in the settings.txt file.

