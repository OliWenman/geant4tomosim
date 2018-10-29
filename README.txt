//////////////////////////////////////////////
//INFORMATION ON HOW TO USE THIS SIMULATION //
//////////////////////////////////////////////

INTRODUCTION
This code is used to simulate X-Ray data for tomography.

Use the "settings.mac" file to tailor the simulation to your needs. If you want visualization of the geometry of your setup, make the "data/Visualization " command to true. This will output a HepARep to be viewed in a HepARep viewer.
Do not use this visualizer with millions of objects or particles as it will take a long time to do the simulation and also crash the viewer. Recommend no more than 1,000 particles, only use it to see if simulation setup is correct.
If you want to get the exact same results each time, you can input your own seed or find what the random seed was by looking in the "SimulationSettings.txt". The seed is random if the input is 0.

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
GEOMETRY SETTINGS FOR THE TARGET

Too add objects into the geometry setup, go into the Geometry.mac file 
Create an object using Target/ObjectName/Dimensions

Avaliable objects are:
-Cube, takes 4 areguments: length of x, y, z and units.
-HollowCube, takes 7 arguments: length of outer box for x, y and z, length of inner box for x, y and z and the unit used.
-Sphere, takes 8 arguments: length of inner radius, outer radius and unit used. Inner phi, outer phi, inner theta, outer theta and unit used(rad or deg).
-Cylinder, takes 7 arguments: length of inner radius, outer radius, height, unit used. Inner phi, outer phi, unit used(rad or deg).

Once you have entered the dimensions, assign its position, rotation and material. You must assign an object each one of these attributes, otherwise you will get an error.
A list of avaliable materials from the database can be found here: 
http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html?highlight=materials

Example:
/Target/Cylinder/Dimensions 0.1 0.15 0.4 mm 0 360 deg 
/Target/Position -0.8 -0.9 0.0 mm
/Target/Rotation 0.0 0.0 0.0 deg
/Target/Material/Database G4_Pb

You can also speficy if you would like the rotation to be offset with a radius and the centre of the offset using:
/Target/OffSet/Radius 0.0 mm			
/Target/OffSet/Centre 0.0 0.0 0.0 mm

The first object you create will become the "master volume", meaning all other volumes created after will rotate around it

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
PHYSICS OPTIONS

Below is a list of avaliable physics packages that are optimised for different needs. In "settings.mac", the command /physics/addPhysics PhysicsSetting , where PhysicsSetting is the name of the physics package, allows you to 
change the simulation to adopt the physic package you need. Simply change the command to the title of any of the below options. For example, "/physics/addPhysics LivermorePhysics". 

PHYSICS OPTIONS
StandardPhysics
- Multiple scattering is simulated with “UseSafety” type of step limitation by combined G4WentzelVIModel and G4eCoulombScatteringModel for all particle types, for of e+- below 100 MeV G4UrbanMscModel is used, 
G4LivermorePhotoElectricModel is used for simulation of the photo-electric effect, the Rayleigh scattering process is enabled below 1 MeV, physics tables are built from 100 eV to 100 TeV, 7 bins per energy decade of physics
tables are used.

StandardPhysics_option1
- Optional EM physics providing fast but less accurate electron transport due to “Simple” method of step limitation by multiple scattering, reduced step limitation by ionisation process, Rayleigh scattering is disabled, 
photo-electric effect is using G4PEEffectFluoModel, and enabled “ApplyCuts” option.
    
StandardPhysics_option2
- Optional EM physics providing fast but less accurate electron transport due to “Simple” method of step limitation by multiple scattering and reduced step limitation by ionisation process, G4Generator2BS angular generator for 
bremsstrahlung, Rayleigh scattering is disabled, and photo-electric effect is using G4PEEffectFluoModel 
    
StandardPhysics_option3
- EM physics for simulation with high accuracy due to “UseDistanceToBoundary” multiple scattering step limitation and usage of G4UrbanMscModel for all charged particles, reduced finalRange parameter of stepping function 
optimized per particle type, alternative model G4KleinNishinaModel for Compton scattering, enabled fluorescence, enabled nuclear stopping, G4Generator2BS angular generator for bremsstrahlung, G4IonParameterisedLossModel 
for ion ionisation, 20 bins per energy decade of physics tables, and 10 eV low-energy limit for tables.
    
StandardPhysics_option4
- Combination of EM models for simulation with high accuracy includes multiple scattering with “UseSafetyPlus” type of step limitation by combined G4WentzelVIModel and G4eCoulombScatteringModel for all particle types, for of 
e+- below 100 MeV G4GoudsmitSaundersonMscModel is used, RangeFactor = 0.02, Scin = 3 (error free stepping near the geometry boundary), reduced finalRange parameter of stepping function optimized per particle type, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4LowEPComptonModel below 20 MeV, G4PenelopeGammaConversionModel below 1 GeV, G4PenelopeIonisationModel for electrons and positrons below 100 keV, G4IonParameterisedLossModel for ion ionisation, G4Generator2BS angular generator for bremsstrahlung, and 20 bins per energy decade of physics tables.
  
*LivermorePhysics  
- Models based on Livermore data bases for electrons and gamma, enabled Rayleigh scattering, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4IonParameterisedLossModel 
for ion ionisation, and 20 bins per energy decade of physics tables. Gamma, e- from 10 eV up to 1 GeV.

LivermorePolarizedPhysics)
- Models for simulation of linear polarized gamma based on Livermore data bases for electrons and gamma.

PhotoElectricEffect
- Using Livermores physics but only the Photoelectric effect, therefore only absorption physics used.

LivemoreGamma
- Using Livermores physics but only the Photoelectriceffect, Raylreigh scattering, and Compton Scattering. 
    
LowEPPhysics
- Models based on Livermore data bases and new model for Compton scattering G4LowEPComptonModel, new low-energy model of multiple scattering G4LowEWenzelMscModel.
    
PenelopePhysics
- Penelope2008 models for electrons, positrons and gamma, enabled Rayleigh scattering, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4IonParameterisedLossModel for 
ion ionisation, and 20 bins per energy decade of physics tables. Gamma, e- , e+ from 100 eV up to 1 GeV (2008 version).
    
StandardPhysicsGS
- Experimental physics with multiple scattering of e+- below 100 MeV simulated by G4GoudsmitSaundersonMscModel is done on top of the default EM physics.
- Experimental physics with multiple scattering of e+- below 100 MeV simulated by a combination of G4WentzelVIModel and G4eCoulombScatteringModel is done on top of the default EM physics.
- Experimental physics with single scattering models instead of multiple scattering is done on top of the default EM physics, for all leptons and hadrons G4eCoulombScatteringModel is used, for ions - G4IonCoulombScatteringModel.
 
EmDNAPhysics
- Low-energy Geant4-DNA physics.

EmDNAPhysics_optionX (where X is 1 to 7)
- Alternative low-energy Geant4-DNA physics constructors. Refer to Geant4-DNA section.

