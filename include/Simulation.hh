#ifndef Simulation_h
#define Simulation_h 1

#include <vector>
#include "globals.hh"
#include <string>
#include "SettingsLog.hh"
#include "Data.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "FluorescenceSD.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"


class SimulationMessenger;
class Data;
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
		void printinfo_pywrapped(unsigned long long int totalparticles, int numberOfimages, int ndarkflatFields);
		
//================================================================================================================		
	
		//Function to run the simulation
        int run_pywrapped(unsigned long long int totalparticles, 
                          std::vector<int>       imageInfo, 
                          double                 rotation_angles);

		//Absorption
		std::vector<int> getAbsorption_pywrapped() {return data -> GetHitData();}
		
		//Fluorescence
		std::vector<
		 std::vector<
		  std::vector<int> > > getFullMappingFluore_pywrapped() {return data -> GetFullMapping();}
		std::vector<int> getFullFieldFluore_pywrapped()         {return data -> GetFluorescence();}
		std::vector<double> getFluoreEneBins_pywrapped()        {return data -> GetEnergyBins();}
		bool FullMappingFluorescence()                          {return data->GetFullMapping_Option();}//DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullMapping();}
		bool FullFieldFluorescence()                            {return data->GetFluorescence_Option();}//DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullField();}
		
		//Diffraction
		std::vector<std::vector<int> > GetDiffractionData() {return data -> GetDiffractionData();}
		
		//Beam energy
		std::vector<int> getBeamEnergy_pywrapped()        {return PGA -> GetBeamEnergy();}
        std::vector<double> getBeamEnergyBins_pywrapped() {return data -> GetEnergyBins();}

//================================================================================================================
	
	    //Get functions
		int getNumFluoreEneBins_pywrapped()            {return data -> GetNumberOfBins();}
		int getNumAbsXpixels_pywrapped()               {return DC->GetAbsorptionDetector()->GetNumberOfxPixels();}
		int getNumAbsYpixels_pywrapped()               {return DC->GetAbsorptionDetector()->GetNumberOfyPixels();}
		std::vector<double> getAbsHalf3Dim_pywrapped() {return DC->GetAbsorptionDetector()->GetHalfDimensions();}
		
		//Set functions
		void setlogfile_pywrapped (std::string path, std::string fileName) {SaveLogPath = path; FileName = fileName;}
		void setSavingTime_pywrapped (double Time)                                    {PGA -> SetSavingTime(Time);}
		
		void SetSeed(long int value)                                {seedCmd = value;}
		void SetVerboseLevel(int value)                             {globalVerbose = value;}

	private:
	    std::string GetStorageUnit(double &storage); 
	    void CalculateStorageSpace(int projections);
	    void Setup();
		void BeamOn(unsigned long long int nParticles);
		void Visualisation();
		unsigned long long int LimitGraphics(unsigned long long int nParticlesint, int nImage, std::string Mode);
		void PrintInformation(SettingsLog log);
		
		//Pointers to different classes
		SimulationMessenger* simMessenger;
		G4RunManager* runManager;
		G4VisManager* visManager;
		G4UImanager* UImanager;
		Data* data;
		DefineMaterials* materials;
		DetectorConstruction* DC;
		PhysicsList* PL;
		PrimaryGeneratorAction* PGA;
		StackingAction* particleManager;
		SteppingAction* stepManager;

		long int seedCmd;

		std::string SaveLogPath;
		std::string FileName;
		
		int globalVerbose;
		
		bool interactiveOn;
		std::vector<std::string> macrofiles;
};

#endif

