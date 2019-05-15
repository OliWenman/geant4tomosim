/*
License info
*/

/*
TomoSim is class used to simulate tomography experiments using the Geant4 toolkit library. This class
is wrapped in Cython and to be controlled from Python. It manages the control of Geant4 and all other 
classes in this library. 

Author: Oliver Jack Wenman
*/

#ifndef Simulation_h
#define Simulation_h 1

#include "MyVectors.hh"
#include "globals.hh"
#include <string>
#include "SettingsLog.hh"
#include "AbsorptionDetector.hh"
#include "AbsorptionSD.hh"
#include "FluorescenceDetector.hh"
#include "FluorescenceSD.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "SampleConstruction.hh"

class G4MPImanager;
class SampleConstruction;
class SimulationMessenger;
class DefineMaterials;
class DetectorConstruction;
class PhysicsList;
class PrimaryGeneratorAction;
class StackingAction;
class SteppingAction;

class G4RunManager;
class G4VisManager;
class G4UImanager;

//Functions that end with "_pywrapped" mean they have been wrapped in Cython.

class Simulation
{
	public:
		Simulation(int verb, bool interactive);
		~Simulation(); 

//================================================================================================================
/*
        Functions that used to execute macro files which contain a list of commands that control the simulation
        settings.
*/

/*
        Execute a list of macro files. Calls the applymacrofile_pywrapped(string) function in a loop.
*/
		void addmacros_pywrapped      (std::vector<std::string> macroFiles);
		
/*
        Reads a macro file, calls the applycommand_pywrapped(string) function when a command is detected.
        Can also be accessed using the command "/tomosim/execute /path/to/macro/file".
*/
		void applymacrofile_pywrapped (std::string macro);
		
/*
        Executes a command. A return value is supplied, indicating if it was successful or not. The return type
        corrosponds to a list of error types found in "CommandStatus.hh". If in interactive mode, the user will
        be able to resupply the command if there was an error executing it. This function is called by
        applymacrofile_pywrapped(string).
*/
		void applycommand_pywrapped   (std::string command);

//================================================================================================================		
/*
        These functions are all used together in a single function "simulate_tomography" in the .pyx file.   
*/

/*
        Used to print information about the settings used to the user before the simulation begins.
*/
		void printinfo_pywrapped(std::string filepath,
		                         unsigned long long int totalparticles, 
		                         int numberOfimages, 
		                         int ndarkflatFields);

/*
        Setup any external visualisation for the simulation 
*/	
		void setupvis_pywrapped(std::string path,
		                        std::string filename);
/*
        simulatetomography_pywrapped is called within a loop, simulating a single set of data each time. 
        The Cython side will then retrieve each appropriate data set and save it in a NeXus file at the 
        end of each loop in the NXtomo format. The rotation_angle will then be changed for the next 
        projection in the loop to simulate. 
*/
        int run_pywrapped(unsigned long long int totalparticles, 
                          int_vector1D           imageInfo, 
                          double                 rotation_angles,
                          double                 zposition);

//================================================================================================================		                          
/*
        Functions designed to be wrapped in Savu
*/
        int runsingleprojection_pywrapped (unsigned long long int totalparticles,
                                           bool   flatfield,
                                           double rotation_angle,
                                           double zposition);    

//================================================================================================================		
        //Functions to get data from detectors

		//Absorption
		int_vector1D    getAbsorption_pywrapped()    {return detectorManager->GetAbsorptionDetector()->GetSensitiveDetector()->GetData(); }
		double_vector1D getAbsHalf3Dim_pywrapped()   {return detectorManager->GetAbsorptionDetector()->GetHalfDimensions();               }
		int             getNumAbsXpixels_pywrapped() {return detectorManager->GetAbsorptionDetector()->GetNumberOfxPixels();              }
		int             getNumAbsYpixels_pywrapped() {return detectorManager->GetAbsorptionDetector()->GetNumberOfyPixels();              }
		
		//Fluorescence
		int_vector3D    getFullMappingFluore_pywrapped() {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->GetFullMapping();    }
		int_vector1D    getFullFieldFluore_pywrapped()   {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->GetFullField();      }
		double_vector1D getFluoreEneBins_pywrapped()     {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->GetEnergyBins();     }
		bool            fluorFMactive_pywrapped()        {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->FullMappingActive(); }
		bool            fluorFFactive_pywrapped()        {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->FullFieldActive();   }
		int             getNumFluorbins_pywrapped()      {return detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->GetNumberOfBins();   }
		
		//Diffraction
		int_vector2D GetDiffractionData() {return int_vector2D()/*data -> GetDiffractionData()*/;}
		
		//Beam energy
		int_vector1D    getBeamEnergy_pywrapped()     {return beamManager -> GetBeamIntensity();  }
        double_vector1D getBeamEnergyBins_pywrapped() {return beamManager -> GetBeamEnergy(); }
        int             getbeambins_pywrapped()       {return beamManager -> GetNumberOfBins();}

//================================================================================================================
		
		//Set pywrapped functions
		void setSavingTime_pywrapped (double Time)                            {beamManager -> SetSavingTime(Time);              }
		
		//Free memory of data
		void freedataMemory_pywrapped() {detectorManager->GetAbsorptionDetector()->GetSensitiveDetector()->FreeMemory();
		                                 detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->FreeMemory();}	
		
		//Set functions
		void SetSeed(long int value);    //{seedCmd = value;}
		void SetVerboseLevel(int value) {globalVerbose = value;}
		
		void CleanGeometry();

	private:
	    void Initialise_dataSets();
	    std::string GetStorageUnit(double &storage); 
	    void CalculateStorageSpace(int projections);
	    void Setup();
		void BeamOn(unsigned long long int nParticles);
		void Visualisation();
		unsigned long long int LimitGraphics(unsigned long long int nParticlesint, int nImage, std::string Mode);
		void PrintInformation(SettingsLog log);
		
		//Pointers to different classes
		G4MPImanager           *g4MPI;
		SimulationMessenger    *simMessenger;
		G4RunManager           *runManager;
		G4VisManager           *visManager;
		G4UImanager            *UImanager;
		DefineMaterials        *materials;
		SampleConstruction     *sampleconstruction;
		DetectorConstruction   *detectorManager;
		PhysicsList            *physicsManager;
		PrimaryGeneratorAction *beamManager;
		StackingAction         *particleManager;
		SteppingAction         *stepManager;

		long int seedCmd;
		bool randseed;
		
		int globalVerbose;
		
		bool interactiveOn;
		string_vector1D macrofiles;
		
		int mpi_rank;
};

#endif

