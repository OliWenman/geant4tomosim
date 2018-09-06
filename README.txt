//////////////////////////////////////////////
//INFORMATION ON HOW TO USE THIS SIMULATION //
//////////////////////////////////////////////

Below is a list of avaliable physics packages that are optimised for different needs. In "settings.mac", the command /physics/addPhysics PhysicsSetting , where PhysicsSetting is the name of the physics package, allows you to change the simulation to adopt the physic package you need. Simply change the command to the title of any of the below options. For example, "/physics/addPhysics EmLivermorePhysics". 

EmStandardPhysics
- Multiple scattering is simulated with “UseSafety” type of step limitation by combined G4WentzelVIModel and G4eCoulombScatteringModel for all particle types, for of e+- below 100 MeV G4UrbanMscModel is used, G4LivermorePhotoElectricModel is used for simulation of the photo-electric effect, the Rayleigh scattering process is enabled below 1 MeV, physics tables are built from 100 eV to 100 TeV, 7 bins per energy decade of physics tables are used.

EmStandardPhysics_option1
- Optional EM physics providing fast but less accurate electron transport due to “Simple” method of step limitation by multiple scattering, reduced step limitation by ionisation process, Rayleigh scattering is disabled, photo-electric effect is using G4PEEffectFluoModel, and enabled “ApplyCuts” option.
    
EmStandardPhysics_option2
- Optional EM physics providing fast but less accurate electron transport due to “Simple” method of step limitation by multiple scattering and reduced step limitation by ionisation process, G4Generator2BS angular generator for bremsstrahlung, Rayleigh scattering is disabled, and photo-electric effect is using G4PEEffectFluoModel 
    
EmStandardPhysics_option3
- EM physics for simulation with high accuracy due to “UseDistanceToBoundary” multiple scattering step limitation and usage of G4UrbanMscModel for all charged particles, reduced finalRange parameter of stepping function optimized per particle type, alternative model G4KleinNishinaModel for Compton scattering, enabled fluorescence, enabled nuclear stopping, G4Generator2BS angular generator for bremsstrahlung, G4IonParameterisedLossModel for ion ionisation, 20 bins per energy decade of physics tables, and 10 eV low-energy limit for tables.
    
EmStandardPhysics_option4
- Combination of EM models for simulation with high accuracy includes multiple scattering with “UseSafetyPlus” type of step limitation by combined G4WentzelVIModel and G4eCoulombScatteringModel for all particle types, for of e+- below 100 MeV G4GoudsmitSaundersonMscModel is used, RangeFactor = 0.02, Scin = 3 (error free stepping near the geometry boundary), reduced finalRange parameter of stepping function optimized per particle type, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4LowEPComptonModel below 20 MeV, G4PenelopeGammaConversionModel below 1 GeV, G4PenelopeIonisationModel for electrons and positrons below 100 keV, G4IonParameterisedLossModel for ion ionisation, G4Generator2BS angular generator for bremsstrahlung, and 20 bins per energy decade of physics tables.
  
EmLivermorePhysics  
- Models based on Livermore data bases for electrons and gamma, enabled Rayleigh scattering, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4IonParameterisedLossModel for ion ionisation, and 20 bins per energy decade of physics tables.

EmLivermorePolarizedPhysics)
- Models for simulation of linear polarized gamma based on Livermore data bases for electrons and gamma.
    
EmLowEPPhysics
- Models based on Livermore data bases and new model for Compton scattering G4LowEPComptonModel, new low-energy model of multiple scattering G4LowEWenzelMscModel.
    
EmPenelopePhysics
- Penelope2008 models for electrons, positrons and gamma, enabled Rayleigh scattering, enabled fluorescence, enabled nuclear stopping, enable accurate angular generator for ionisation models, G4IonParameterisedLossModel for ion ionisation, and 20 bins per energy decade of physics tables.
    
EmStandardPhysicsGS
- Experimental physics with multiple scattering of e+- below 100 MeV simulated by G4GoudsmitSaundersonMscModel is done on top of the default EM physics.
- Experimental physics with multiple scattering of e+- below 100 MeV simulated by a combination of G4WentzelVIModel and G4eCoulombScatteringModel is done on top of the default EM physics.
- Experimental physics with single scattering models instead of multiple scattering is done on top of the default EM physics, for all leptons and hadrons G4eCoulombScatteringModel is used, for ions - G4IonCoulombScatteringModel.
 
EmDNAPhysics
- Low-energy Geant4-DNA physics.

EmDNAPhysics_optionX (where X is 1 to 7)
- Alternative low-energy Geant4-DNA physics constructors. Refer to Geant4-DNA section.

