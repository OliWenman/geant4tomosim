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

//Functions that end with "_pywrapped" mean they have been wrapped in Cython and can be called from python

class Simulation
{
	public:
		Simulation();
		Simulation(int verb, bool interactive);
		~Simulation(); 

//================================================================================================================

		//Apply a list of macro files or commands to the simulation
		void addmacros_pywrapped      (std::vector<std::string> macroFiles);
		void applycommand_pywrapped   (std::string command);
		void applymacrofile_pywrapped (std::string macro);
		
		//Print information about the simulation to the user. Amount of info depends on verbose setting
		void printinfo_pywrapped(std::string filepath,
		                         unsigned long long int totalparticles, 
		                         int numberOfimages, 
		                         int ndarkflatFields);
		
//================================================================================================================		
	
		//Function to run the simulation
        int run_pywrapped(unsigned long long int totalparticles, 
                          int_vector1D           imageInfo, 
                          double                 rotation_angles);
                          
        int runsingleprojection_pywrapped (unsigned long long int totalparticles,
                                           bool   flatfield,
                                           double rotation_angle,
                                           double zposition);    

        //Functions to get data from detectors

		//Absorption
		int_vector1D    getAbsorption_pywrapped()    {return DC->GetAbsorptionDetector()->GetSensitiveDetector()->GetData(); }
		double_vector1D getAbsHalf3Dim_pywrapped()   {return DC->GetAbsorptionDetector()->GetHalfDimensions();               }
		int             getNumAbsXpixels_pywrapped() {return DC->GetAbsorptionDetector()->GetNumberOfxPixels();              }
		int             getNumAbsYpixels_pywrapped() {return DC->GetAbsorptionDetector()->GetNumberOfyPixels();              }
		
		//Fluorescence
		int_vector3D    getFullMappingFluore_pywrapped() {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->GetFullMapping();    }
		int_vector1D    getFullFieldFluore_pywrapped()   {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->GetFullField();      }
		double_vector1D getFluoreEneBins_pywrapped()     {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->GetEnergyBins();     }
		bool            fluorFMactive_pywrapped()        {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullMappingActive(); }
		bool            fluorFFactive_pywrapped()        {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullFieldActive();   }
		int             getNumFluorbins_pywrapped()      {return DC->GetFluorescenceDetector()->GetSensitiveDetector()->GetNumberOfBins();   }
		
		//Diffraction
		int_vector2D GetDiffractionData() {/*return data -> GetDiffractionData();*/}
		
		//Beam energy
		int_vector1D    getBeamEnergy_pywrapped()     {return PGA -> GetBeamIntensity();  }
        double_vector1D getBeamEnergyBins_pywrapped() {return PGA -> GetBeamEnergy(); }
        int             getbeambins_pywrapped()       {return PGA -> GetNumberOfBins();}

//================================================================================================================
		
		//Set pywrapped functions
		void setSavingTime_pywrapped (double Time)                            {PGA -> SetSavingTime(Time);              }
		
		//Free memory of data
		void freedataMemory_pywrapped() {DC->GetAbsorptionDetector()->GetSensitiveDetector()->FreeMemory();
		                                 DC->GetFluorescenceDetector()->GetSensitiveDetector()->FreeMemory();}	
		
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
		SimulationMessenger    *simMessenger;
		G4RunManager           *runManager;
		G4VisManager           *visManager;
		G4UImanager            *UImanager;
		DefineMaterials        *materials;
		SampleConstruction     *sampleconstruction;
		DetectorConstruction   *DC;
		PhysicsList            *PL;
		PrimaryGeneratorAction *PGA;
		StackingAction         *particleManager;
		SteppingAction         *stepManager;

		long int seedCmd;
		bool randseed;
		
		int globalVerbose;
		
		bool interactiveOn;
		string_vector1D macrofiles;
};

#endif

